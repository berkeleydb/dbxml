//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#include "NsHandlerBase.hpp"
#include "NsEvent.hpp"
#include "NsConstants.hpp"

using namespace DbXml;
#ifndef DBXML_BULK
/*
	Initially assume all nodes will be 4 bytes larger than they are
	when the reserve node is added to the database.
*/
const size_t INITIAL_PREDICTION = 4;
/* 
	Since it is usually better to over estimate the size of a node then
	to under estimate it, pad node size predictions by 4 bytes.
*/
const size_t PADDING = 4;

SizePredictionStatistics::SizePredictionStatistics() 
{ 
  ::memset(this, 0, sizeof(SizePredictionStatistics));
  _prediction = INITIAL_PREDICTION;
  
}

int64_t SizePredictionStatistics::getPrediction() const
{
	return _prediction/(_count + 1);
}

int64_t SizePredictionStatistics::getAverage() const
{
	if (_count) return (_over + _under)/_count;
	return 0;
}

double SizePredictionStatistics::getPercentUnder() const
{
	if (_count) return 100.0 * ((double)_undercount/(double)_count);
	return 0.0;
}

int64_t SizePredictionStatistics::getAverageUnder() const
{
	if (_undercount) return _under/_undercount;
	return 0;
}

double SizePredictionStatistics::getPercentOver() const
{
	if (_count) return 100.0 * ((double)_overcount/(double)_count);
	return 0.0;
}

int64_t SizePredictionStatistics::getAverageOver() const
{
	if (_overcount) return _over/_overcount;
	return 0;
}

void SizePredictionStatistics::increment(int64_t error, int64_t correctPrediction) 
{
	_count++;
	_prediction += correctPrediction;
	if (error > 0) {
		_over += error;
		_overcount++;
	} else if (error < 0) {
		_under += error;
		_undercount++;
	}
}

void SizePredictionStatistics::reset()
{
	_count = _under = _over = _undercount = _overcount = 0;
	_prediction = INITIAL_PREDICTION;
}

#endif

NsHandlerBase::NsHandlerBase(NsDocument *doc,
			     EventWriter *ewriter)
	: _doc(doc), _ewriter(ewriter),
	  _current(0), _previous(0), _depth(0),
	  _lastWasText(false), _textList(0)
{}

NsHandlerBase::~NsHandlerBase()
{
	NsNode *node;
	while(_current) {
		node = _current;
		_current = node->getParent();
		node->release();
	}
	if(_previous) {
		_previous->release();
	}
	if (_textList)
		NsNode::freeTextList(_textList);
}

void
NsHandlerBase::nextId(NsFullNid *id)
{
	_nidGen.nextId(id);
}

/*
 * Generic parts of node creation --
 */
void
NsHandlerBase::startElem(NsNode *node,
			 const xmlbyte_t *baseName,
			 const char *uri, // may be null
			 const char *prefix,
			 bool isDonated)
{
	int uriIndex = 0;
	int prefixIndex = NS_NOPREFIX;
	//
	// Structural initialization
	//
	++_depth;
	node->setParent(_current);
	node->setLevel(_depth);
	if (_previous) {
		node->setNextPrev(_previous);
		NsNode *tprev = _previous;
		_previous = 0;
#ifdef DBXML_BULK
		_doc->completeNode(tprev, 0);
#else
		size_t size = gatherStatistics(tprev);
		_doc->completeNode(tprev, size);
#endif
	}
	
	//
	// element-specific initialization
	//
	//
	// Update namespace info
	//
	if (uri) {
		uriIndex = _doc->addIDForString(uri, ::strlen(uri));
		node->setFlag(NS_HASURI);
		// cannot be prefix without uri
		if (prefix) {
			prefixIndex = _doc->addIDForString(prefix,
							   ::strlen(prefix));
			node->setFlag(NS_NAMEPREFIX);
		}
	}
	
	// name
	node->setUriIndex(uriIndex);
	node->setNamePrefix(prefixIndex);
	size_t nlen = NsUtil::nsStringLen(baseName);
	NsNode::createText(node->getNameText(),
			   baseName, nlen, isDonated);

	//
	// conditional initialization
	//
	if (_current) {
		DBXML_ASSERT(node->getParent() == _current);
#ifndef DBXML_BULK
		/*
		  If this is the first child of the parent then space needs to be
		  reserved in the database for the parent node.  At the moment the
		  parent's name, attributes, and text before the first child are 
		  known.  The last child/descendant, number of children, and text
		  after the first child are unknown, and the size of those needs
		  to be guessed.  The parent will not get much bigger unless there
		  is a lot of additional text, which happens in mixed content XML.
		  When text is moved to another database then this will not be an 
		  issue.
		 */
		if(!_current->hasChildElem()) {
			const NsFormat *format = _current->getFormat();
			size_t size = format->marshalNodeData(_current, NULL, true);
			_sizePredictions.push_back(size);
			size += (size_t)_stats.getPrediction();
			_sizePredictions.push_back(size);
			_doc->reserveNode(_current->getNid(), size);
		}
#endif
		// tell node we're about to start it up.  It's responsible
		// for copying parent/child NIDs if necessary
		_textList = _current->startElem(node, _textList);
	}
	_lastWasText = false;

	// reset current, parse-global state
	_current = node;

	// addition of attributes and calling of event handler
	// are done in the calling instance
}

// used for end of partial documents updates (e.g. NsUpdate)
void
NsHandlerBase::end()
{
	endElem();
	if (_previous) {
		NsNode *tprev = _previous;
		_previous = 0;
#ifdef DBXML_BULK
		_doc->completeNode(tprev, 0);
#else
		size_t size = gatherStatistics(tprev);
		_doc->completeNode(tprev, size);
#endif
	}
	// this is safe to use for partial updates because
	// the NsDocument's docInfo_ field will be empty
	// and it will only serve to flush not-yet-written
	// records
	_doc->completeDocument();
		
}

#if defined(DEBUG) && !defined(DBXML_BULK)
	static int64_t global_average = 0;
	static int64_t global_count = 0;
#endif

void
NsHandlerBase::endDoc()
{
	end();
#ifndef DBXML_BULK
	if(Log::isLogEnabled(Log::C_NODESTORE, Log::L_DEBUG)) {
		std::ostringstream oss;
		oss << "Over Estimated Node Size " << (double) _stats.getPercentOver() << "%\n";
		oss << "Average Bytes Over Estimated " << (int) _stats.getAverageOver() << "\n";
		oss << "Under Estimated Node Size " << _stats.getPercentUnder() << "%\n";
		oss << "Average Bytes Under Estimated " << (int)_stats.getAverageUnder() << "\n";
		oss << "Average Error in Bytes " << (int)_stats.getAverage() << "\n";
#ifdef DEBUG
		global_average += _stats.getAverage();
		oss << "Global Average Error in Bytes " 
			<< (int)(global_average/++global_count) << "\n";
#endif
		Log::log((_doc->getDocDb() ? _doc->getDocDb()->getEnvironment() : 0),
			 Log::C_NODESTORE, Log::L_DEBUG, oss.str().c_str());
	}
#endif
}

void
NsHandlerBase::endElem()
{
	// don't lose track of _current until safe from exceptions...
	NsNode *node = _current;
	
	// tell node that we're about to be done with it, passing
	// parent along
	_textList = node->endElem(node->getParent(), _previous, _textList);
	if (_previous) {
		NsNode *tprev = _previous;
		_previous = 0;
#ifdef DBXML_BULK
		_doc->completeNode(tprev, 0); // can throw deadlock
#else
		size_t size = gatherStatistics(tprev);
		_doc->completeNode(tprev, size); // can throw deadlock
#endif
	}
	_previous = node;
	_current = node->getParent();

	// pop depth counter, but not if was doc node
	// which means _current is null
	if (_current)
		--_depth;

	// must reset current text after element ends
	_lastWasText = false;
}

//
// Add text to the current text list, allocating one if necessary
//  text -- pointer to text
//  len -- length in bytes (utf-8), not including null.
//  textType -- type of text, plus, optionally NS_ENTITY_CHK flag (means
//    checks must use logical operators).
//  isDonated -- true if text string is donated memory
//
void
NsHandlerBase::addText(const xmlbyte_t *text, size_t len,
		       uint32_t textType, bool isDonated)
{
	DBXML_ASSERT(_current);
	if (_lastWasText && (nsTextType(textType) == NS_TEXT)) {
		/* can coalesce */
		NsNode::coalesceText(_textList,
				     text, len, isDonated);
		if (isDonated) {
			NsUtil::deallocate((void*)text);
		}
	} else {
		// textType may include NS_ENTITY_CHK, if donated.
		_textList = _current->addText(_textList, text, len,
					      textType, isDonated);
		_lastWasText = (nsTextType(textType) == NS_TEXT);
	}
	if (textType & NS_ENTITY_CHK)
		_textList->tl_text[_textList->tl_ntext - 1].te_type |=
			NS_ENTITY_CHK;
}

//
// Add processing instruction to the current node.
//  target -- pointer to target text
//  data -- pointer to data text
//  isDonated -- true if text string is donated memory
//  len -- length in bytes (utf-8) of both strings, including null.
//
void
NsHandlerBase::addPI(const xmlbyte_t *target, const xmlbyte_t *data,
		     bool isDonated, size_t len)
{
	DBXML_ASSERT(_current);

	_textList = _current->addPI(_textList, target, data,
				    isDonated, len);

	_lastWasText = false;
}

#ifndef DBXML_BULK
size_t NsHandlerBase::gatherStatistics(const NsNode *node)
{
	if(node->hasChildElem() && _sizePredictions.size()) {
		const NsFormat *format = node->getFormat();
		size_t size = format->marshalNodeData(node, NULL, true);
		int64_t error = (int64_t)_sizePredictions.back() - (int64_t)size;
		_sizePredictions.pop_back();
		_stats.increment(error, (int64_t)size - (int64_t)_sizePredictions.back());
		_sizePredictions.pop_back();
		return size;
	}
	return 0;
}
#endif
