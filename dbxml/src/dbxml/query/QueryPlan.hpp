//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2009 Oracle.  All rights reserved.
//
//

#ifndef __QUERYPLAN_HPP
#define	__QUERYPLAN_HPP

#include <vector>
#include <set>
#include <string>
#include <xqilla/framework/XPath2MemoryManager.hpp>
#include <xqilla/ast/StaticAnalysis.hpp>
#include <xqilla/ast/LocationInfo.hpp>

#include "ImpliedSchemaNode.hpp"
#include "Statistics.hpp"
#include "IndexEntry.hpp"
#include "db.h"

#define LOG_RULES 1
#define ARGUMENT_CUTOFF_FACTOR 2
#define RULE_CUTOFF_FACTOR 1.5
#define MAX_ALTERNATIVES 7
#define MAX_COMBINATIONS 50

class Result;
class ASTNode;
class DynamicContext;
class StaticTyper;

namespace DbXml
{

class ValueQP;
class KeyStatistics;
class Log;
class QueryExecutionContext;
class QPValue;
class IndexLookups;
class QueryPlanRoot;
class StructuralStats;
class StructuralStatsCache;
class ContainerBase;
class QueryPlanOptimizer;
class QueryPlan;
	
typedef std::vector<const QPValue*> ValueDefs;
typedef std::set<const QueryPlanRoot*> QPRSet;
typedef std::vector<QueryPlan*> QueryPlans;

std::string shorten(const std::string &name, unsigned int length);

class OptimizationContext {
public:
	enum Phase {
		RESOLVE_INDEXES = 1,  // Resolve indexes, pull forward document indexes
		ALTERNATIVES = 2,     // Apply speculative optimisations and cost analyze them
		REMOVE_REDUNDENTS = 3 // Push back and remove redundent document indexes
	};

	OptimizationContext(Phase ph, DynamicContext *cn, QueryPlanOptimizer *qpo, ContainerBase *c = 0)
		: phase_(ph), context_(cn), qpo_(qpo), container_(c), isFetched_(false), checkForSS_(false) {}

	Phase getPhase() const { return phase_; }

	DynamicContext *getContext() const { return context_; }
	XPath2MemoryManager *getMemoryManager() const;

	QueryPlanOptimizer *getQueryPlanOptimizer() const { return qpo_; }
	void setQueryPlanOptimizer(QueryPlanOptimizer *qpo) { qpo_ = qpo; }

	ContainerBase *getContainerBase() const { return container_; }

	Transaction *getTransaction() const;
	OperationContext &getOperationContext() const;
	const IndexSpecification &getIndexSpecification() const;
	const Log &getLog() const;

	bool checkForSS() const { return checkForSS_; }
	void setCheckForSS(bool val) { checkForSS_ = val; }

private:
	Phase phase_;
	DynamicContext *context_;
	QueryPlanOptimizer *qpo_;
	ContainerBase *container_;
	mutable IndexSpecification is_;
	mutable bool isFetched_;
	bool checkForSS_;
};

class QueryPlan : public LocationInfo
{
public:
	virtual ~QueryPlan() {}
	typedef enum {
		// Index lookups
		PATHS,
		PRESENCE,
		VALUE,
		RANGE,
		EMPTY,
		SEQUENTIAL_SCAN,

		// Other sources
		CONTEXT_NODE,
		VARIABLE,
		COLLECTION,
		DOC,
		AST_TO_QP,

		// Set operations
		UNION,
		INTERSECT,
		EXCEPT,

		// QueryPlan management
		DECISION_POINT,
		DECISION_POINT_END,
		BUFFER,
		BUFFER_REF,

		// Filters
		VALUE_FILTER,
		PREDICATE_FILTER,
		NODE_PREDICATE_FILTER,
		NEGATIVE_NODE_PREDICATE_FILTER,
		NUMERIC_PREDICATE_FILTER,
		LEVEL_FILTER,
		DOC_EXISTS,

		// Conventional navigation
		STEP,

		// Structural joins
		DESCENDANT,
		DESCENDANT_OR_SELF,
		ANCESTOR,
		ANCESTOR_OR_SELF,
		ATTRIBUTE,
		CHILD,
		ATTRIBUTE_OR_CHILD,
		PARENT,
		PARENT_OF_ATTRIBUTE,
		PARENT_OF_CHILD,

		// Debugging
		DEBUG_HOOK
	} Type;

	enum Flags {
		SKIP_PUSH_BACK_JOIN                   = 0x00000001,
		SKIP_REVERSE_JOIN                     = 0x00000002,
		SKIP_NESTED_PREDICATES                = 0x00000004,
		SKIP_LEFT_TO_PREDICATE                = 0x00000010,
		SKIP_RIGHT_LOOKUP_TO_LEFT_STEP        = 0x00000020,
		SKIP_TO_NEGATIVE_PREDICATE            = 0x00000040,
		SKIP_SWAP_STEP                        = 0x00000100
	};

	QueryPlan(Type t, u_int32_t flags, XPath2MemoryManager *mm)
		: memMgr_(mm), type_(t), flags_(flags), _src(mm) {}

	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const = 0;
	virtual void release() = 0;

	Type getType() const
	{
		return type_;
	}

	u_int32_t getFlags() const { return flags_; }
	void setFlags(u_int32_t f) { flags_ = f; }
	void addFlag(Flags flag) { flags_ |= flag; }

	virtual void staticTypingLite(StaticContext *context) = 0;
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper) = 0;
	virtual const StaticAnalysis &getStaticAnalysis() const { return _src; }

	virtual QueryPlan *optimize(OptimizationContext &opt) = 0;
	virtual void createCombinations(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &combinations) const;
	virtual void applyConversionRules(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives);

	void createAlternatives(unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives) const;
	void createReducedAlternatives(double cutOffFactor, unsigned int maxAlternatives, OptimizationContext &opt, QueryPlans &alternatives) const;
	QueryPlan *chooseAlternative(OptimizationContext &opt, const char *name) const;

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const = 0;
	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const = 0;

	/** Returns the QueryPlanRoot objects from the PathsQP in this QueryPlan */
	virtual void findQueryPlanRoots(QPRSet &qprset) const = 0;
	/// Returns true if it's sure. Returns false if it doesn't know
	virtual bool isSubsetOf(const QueryPlan *o) const = 0;
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const = 0;
	virtual std::string toString(bool brief = true) const = 0;

	static void log(QueryExecutionContext &qec, const std::string &message);
	void logQP(const Log &log, const std::string &title, const QueryPlan *qp,
		OptimizationContext::Phase phase = (OptimizationContext::Phase)-1) const;

	std::string logBefore(const QueryPlan *before) const;
	void logTransformation(const Log &log, const std::string &transform, const std::string &before,
		const QueryPlan *result) const;
	void logTransformation(const Log &log, const std::string &transform, const QueryPlan *before,
		const std::string &result) const;
	void logTransformation(const Log &log, const std::string &transform, const QueryPlan *before,
		const QueryPlan *result) const;
	void logTransformation(const Log &log, const QueryPlan *transformed) const;

	void logCost(QueryExecutionContext &qec, Cost &cost, const KeyStatistics *stats) const;
	void logCost(QueryExecutionContext &qec, const std::string &name, Cost &cost) const;
	void logCost(QueryExecutionContext &qec, const std::string &name, double pages) const;

protected:
	void logLegend(const Log &log) const;

	XPath2MemoryManager *memMgr_;
	Type type_;
	u_int32_t flags_;
	StaticAnalysis _src;

private:
	QueryPlan(const QueryPlan &);
	QueryPlan &operator=(const QueryPlan &);
};

class OperationQP : public QueryPlan
{
public:
	typedef std::vector<QueryPlan*,XQillaAllocator<QueryPlan*> > Vector;

	OperationQP(QueryPlan::Type type, u_int32_t flags, XPath2MemoryManager *mm)
		: QueryPlan(type, flags, mm), args_(XQillaAllocator<QueryPlan*>(mm)) {}

	const Vector &getArgs() const
	{
		return args_;
	}
	virtual QueryPlan *addArg(QueryPlan *o);

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSupersetOf(const QueryPlan *o) const = 0;

	virtual QueryPlan *compress();

protected:
	QueryPlan *dissolve();

	Vector args_;
};

class PathsQP : public QueryPlan
{
public:
	typedef std::vector<ImpliedSchemaNode*,XQillaAllocator<ImpliedSchemaNode*> > Paths;

	PathsQP(const Paths &paths, XPath2MemoryManager *mm);
	PathsQP(const ImpliedSchemaNode::Vector &paths, XPath2MemoryManager *mm);
	virtual ~PathsQP() {}

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const { return 0; }

	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const { return Cost(); }

	const Paths &getPaths() const { return paths_; }
	void addPaths(const Paths &o);

	virtual void findQueryPlanRoots(QPRSet &qprset) const;

	virtual bool isSubsetOf(const QueryPlan *o) const { return false; }
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	static QueryPlan *createStep(ImpliedSchemaNode *path, const LocationInfo *location, XPath2MemoryManager *mm);
	static bool getStepNames(const ImpliedSchemaNode *child, const char *&parentName, const char *&childName, XPath2MemoryManager *mm);

protected:
	QueryPlan *dissolve();

	Paths paths_;
};

class QPValue {
public:
 	QPValue(Syntax::Type syntax, const char *value, size_t vlen, bool generalComp, XPath2MemoryManager *mm);
	QPValue(Syntax::Type syntax, const ASTNode *di, bool generalComp, XPath2MemoryManager *mm);
	QPValue(const QPValue &o, XPath2MemoryManager *mm);

	void release();

	Syntax::Type getSyntax() const {
		return syntax_;
	}
	void setSyntax(Syntax::Type s) {
		syntax_ = s;
	}
	const char *getValue() const {
		return value_;
	}
	size_t getLength() const {
		return len_;
	}
	const ASTNode *getASTNode() const {
		return di_;
	}
	void setASTNode(const ASTNode *di) {
		di_ = di;
	}
	bool isGeneralComp() const {
		return generalComp_;
	}
	std::string asString() const;
	bool equals(const QPValue &o) const;

private:
	QPValue(const QPValue &o);
	QPValue &operator=(const QPValue &);

	Syntax::Type syntax_;
	const char *value_;
	size_t len_;
	const ASTNode *di_;
	bool generalComp_;
	XPath2MemoryManager *mm_;
};

/// Needed because Key can't be used inside a memory managed object
class QPKey {
public:
	QPKey();
	QPKey(Syntax::Type syntax);

	void setNodeLookup(bool n) {
		nodeLookup_ = n;
	}
	const Index &getIndex() const {
		return index_;
	}
	Index &getIndex() {
		return index_;
	}
	const NameID &getID1() const {
		return id1_;
	}
	NameID &getID1() {
		return id1_;
	}
	void setID1(const NameID &id) {
		id1_ = id;
	}
	const NameID &getID2() const {
		return id2_;
	}
	NameID &getID2() {
		return id2_;
	}
	void setID2(const NameID &id) {
		id2_ = id;
	}

	void setIDsFromNames(OperationContext &context, const ContainerBase &container,
		const char *parentUriName, const char *childUriName);

	Key createKey(int timezone) const;
	Key createKey(const QPValue &value, int timezone) const;
	Key createKey(const char *value, size_t length, int timezone) const;

private:
	bool nodeLookup_;
	Index index_;
	NameID id1_;
	NameID id2_;
};

class PresenceQP : public QueryPlan
{
public:
	PresenceQP(ImpliedSchemaNode::Type type, const char *parent, const char *child,
		bool documentIndex, ImpliedSchemaNode *isn, u_int32_t flags,
		XPath2MemoryManager *mm);

	bool isDocumentIndex() const { return documentIndex_; }

	const char *getParentName() const { return parentUriName_; }
	const char *getChildName() const { return childUriName_; }
	bool isParentSet() const { return parentUriName_ != 0; }
	ImpliedSchemaNode::Type getNodeType() const { return nodeType_; }
	DbWrapper::Operation getOperation() const { return operation_; }
	const QPKey &getKey() const { return key_; }

	ImpliedSchemaNode *getImpliedSchemaNode() const { return isn_; }
	ContainerBase *getContainerBase() const { return container_; }

	NodeIterator *lookupNodeIterator(DbWrapper::Operation operation, Key &key,
		DynamicContext *context) const;
	Cost lookupCost(DbWrapper::Operation operation, Key &key,
		OperationContext &context, QueryExecutionContext &qec) const;

	virtual void getKeys(IndexLookups &keys, DynamicContext *context) const;
	virtual void getKeysForCost(IndexLookups &keys, DynamicContext *context) const;

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);

	QueryPlan *simpleLookupOptimize(OptimizationContext &opt);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;
	virtual StructuralStats getStructuralStats(OperationContext &oc, StructuralStatsCache &cache,
		const NameID &nameID, bool ancestor) const;

	ImpliedSchemaNode::Type getReturnType() const;
	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

protected:
	PresenceQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type, const char *parent,
		const char *child, bool documentIndex, const QPKey &key, DbWrapper::Operation operation,
		ImpliedSchemaNode *isn, ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm);
	bool resolveIndexes(const ContainerBase &container, const IndexSpecification &is, bool useSubstring);
	bool indexesResolved() const;

	void logIndexUse(const Log &log, const Index &index, const DbWrapper::Operation op) const;

	ImpliedSchemaNode *isn_;

	ImpliedSchemaNode::Type nodeType_;
	const char *parentUriName_;
	const char *childUriName_;
	bool documentIndex_;

	ContainerBase *container_;

	QPKey key_;
	DbWrapper::Operation operation_;

	mutable Cost cost_;
	mutable bool costSet_;
};

class ValueQP : public PresenceQP
{
public:
	ValueQP(ImpliedSchemaNode::Type type,
		const char *parent, const char *child, bool documentIndex,
		DbWrapper::Operation operation, bool generalComp,
		Syntax::Type syntax, const ASTNode *value,
		ImpliedSchemaNode *isn, u_int32_t flags, XPath2MemoryManager *mm);
	ValueQP(ImpliedSchemaNode::Type type,
		const char *parent, const char *child, bool documentIndex,
		Syntax::Type syntax, DbWrapper::Operation operation,
		bool generalComp, const char *value, size_t vlen,
		ImpliedSchemaNode *isn, u_int32_t flags, XPath2MemoryManager *mm);
	ValueQP(QueryPlan::Type qpType, ImpliedSchemaNode::Type type,
		const char *parent, const char *child, bool documentIndex,
		const QPKey &key, DbWrapper::Operation operation,
		const QPValue &value, ImpliedSchemaNode *isn,
		ContainerBase *cont, u_int32_t flags, XPath2MemoryManager *mm);

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);

	virtual void getKeys(IndexLookups &keys, DynamicContext *context) const;
	virtual void getKeysForCost(IndexLookups &keys, DynamicContext *context) const;

	virtual QueryPlan *resolveValues(const Log &log, DynamicContext *context);

	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	const QPValue &getValue() const {
		return value_;
	}

protected:
	bool isSubsetOfValue(const PresenceQP *step, DbWrapper::Operation myOp, DbWrapper::Operation hisOp) const;
	void getKeysImpl(IndexLookups &keys, const char *value, size_t length, int timezone) const;
	QueryPlan *resolveFromResult(Result &result, const Log &log, DynamicContext *context, const LocationInfo *location);
	bool resolveIndexes(const ContainerBase &container, const IndexSpecification &is);

	QPValue value_;
};

class RangeQP : public ValueQP
{
public:
	RangeQP(const ValueQP *gt, const ValueQP *lt, XPath2MemoryManager *mm);

	DbWrapper::Operation getOperation2() const { return operation2_; }

	virtual void staticTypingLite(StaticContext *context);
	virtual QueryPlan *staticTyping(StaticContext *context, StaticTyper *styper);
	virtual QueryPlan *optimize(OptimizationContext &opt);

	virtual NodeIterator *createNodeIterator(DynamicContext *context) const;

	virtual Cost cost(OperationContext &context, QueryExecutionContext &qec) const;

	virtual void findQueryPlanRoots(QPRSet &qprset) const;
	virtual bool isSubsetOf(const QueryPlan *o) const;
	virtual QueryPlan *copy(XPath2MemoryManager *mm = 0) const;
	virtual void release();
	virtual std::string printQueryPlan(const DynamicContext *context, int indent) const;
	virtual std::string toString(bool brief = true) const;

	const QPValue &getValue2() const {
		return value2_;
	}

	ImpliedSchemaNode *getImpliedSchemaNode2() const { return isn2_; }

protected:
	RangeQP(ImpliedSchemaNode::Type type,
		const char *parent, const char *child, bool documentIndex,
		const QPKey &key1, DbWrapper::Operation operation1,
		const QPValue &value1, DbWrapper::Operation operation2,
		const QPValue &value2, ImpliedSchemaNode *isn1,
		ImpliedSchemaNode *isn2, ContainerBase *cont,
		u_int32_t flags, XPath2MemoryManager *mm);
	bool resolveIndexes(const ContainerBase &container, const IndexSpecification &is);

	ImpliedSchemaNode *isn2_;

	QPValue value2_;
	DbWrapper::Operation operation2_;
};

struct CostSortItem {
	CostSortItem(double cost, bool hasSS) : qp_(0), cost_(0, cost), hasSS_(hasSS) {}
	CostSortItem(QueryPlan *qp, OperationContext &oc, QueryExecutionContext &qec, bool checkForSS);

	bool operator<(const CostSortItem &o) const;

	QueryPlan *qp_;
	Cost cost_;
	bool hasSS_;
};

}

#endif
