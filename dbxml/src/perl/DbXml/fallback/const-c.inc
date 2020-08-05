#define PERL_constant_NOTFOUND	1
#define PERL_constant_NOTDEF	2
#define PERL_constant_ISIV	3
#define PERL_constant_ISNO	4
#define PERL_constant_ISNV	5
#define PERL_constant_ISPV	6
#define PERL_constant_ISPVN	7
#define PERL_constant_ISSV	8
#define PERL_constant_ISUNDEF	9
#define PERL_constant_ISUV	10
#define PERL_constant_ISYES	11

#ifndef NVTYPE
typedef double NV; /* 5.6 and later define NVTYPE, and typedef NV to it.  */
#endif
#ifndef aTHX_
#define aTHX_ /* 5.6 or later define this for threading support.  */
#endif
#ifndef pTHX_
#define pTHX_ /* 5.6 or later define this for threading support.  */
#endif

static int
constant_14 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "XmlValue::DATE" "XmlValue::NODE" "XmlValue::NONE" "XmlValue::TIME" */
  /* Offset 12 gives the best switch position.  */
  switch (name[12]) {
  case 'D':
    if (memEQ(name, "XmlValue::NODE", 14)) {
    /*                           ^        */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "XmlValue::TIME", 14)) {
    /*                           ^        */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::TIME;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "XmlValue::NONE", 14)) {
    /*                           ^        */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::NONE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "XmlValue::DATE", 14)) {
    /*                           ^        */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::DATE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_15 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "XmlValue::FLOAT" "XmlValue::G_DAY" "XmlValue::QNAME" */
  /* Offset 10 gives the best switch position.  */
  switch (name[10]) {
  case 'F':
    if (memEQ(name, "XmlValue::FLOAT", 15)) {
    /*                         ^           */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::FLOAT;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "XmlValue::G_DAY", 15)) {
    /*                         ^           */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::G_DAY;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'Q':
    if (memEQ(name, "XmlValue::QNAME", 15)) {
    /*                         ^           */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::QNAME;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_16 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::LEVEL_ALL" "XmlValue::BINARY" "XmlValue::DOUBLE"
     "XmlValue::G_YEAR" "XmlValue::STRING" */
  /* Offset 10 gives the best switch position.  */
  switch (name[10]) {
  case 'B':
    if (memEQ(name, "XmlValue::BINARY", 16)) {
    /*                         ^            */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::BINARY;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "XmlValue::DOUBLE", 16)) {
    /*                         ^            */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::DOUBLE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "DbXml::LEVEL_ALL", 16)) {
    /*                         ^            */
      *iv_return = (IV)DbXml::LEVEL_ALL;
      return PERL_constant_ISUV;
    }
    break;
  case 'G':
    if (memEQ(name, "XmlValue::G_YEAR", 16)) {
    /*                         ^            */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::G_YEAR;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "XmlValue::STRING", 16)) {
    /*                         ^            */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::STRING;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_17 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::LEVEL_INFO" "DbXml::LEVEL_NONE" "XmlEventType::DTD"
     "XmlValue::ANY_URI" "XmlValue::BOOLEAN" "XmlValue::DECIMAL"
     "XmlValue::G_MONTH" */
  /* Offset 16 gives the best switch position.  */
  switch (name[16]) {
  case 'D':
    if (memEQ(name, "XmlEventType::DT", 16)) {
    /*                               D      */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::DTD;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "DbXml::LEVEL_NON", 16)) {
    /*                               E      */
      *iv_return = (IV)DbXml::LEVEL_NONE;
      return PERL_constant_ISUV;
    }
    break;
  case 'H':
    if (memEQ(name, "XmlValue::G_MONT", 16)) {
    /*                               H      */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::G_MONTH;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "XmlValue::ANY_UR", 16)) {
    /*                               I      */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::ANY_URI;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "XmlValue::DECIMA", 16)) {
    /*                               L      */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::DECIMAL;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "XmlValue::BOOLEA", 16)) {
    /*                               N      */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::BOOLEAN;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "DbXml::LEVEL_INF", 16)) {
    /*                               O      */
      *iv_return = (IV)DbXml::LEVEL_INFO;
      return PERL_constant_ISUV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_18 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::LEVEL_DEBUG" "DbXml::LEVEL_ERROR" "XmlIndexLookup::EQ"
     "XmlIndexLookup::GT" "XmlIndexLookup::LT" "XmlValue::DURATION"
     "XmlValue::NOTATION" */
  /* Offset 17 gives the best switch position.  */
  switch (name[17]) {
  case 'G':
    if (memEQ(name, "DbXml::LEVEL_DEBU", 17)) {
    /*                                G      */
      *iv_return = (IV)DbXml::LEVEL_DEBUG;
      return PERL_constant_ISUV;
    }
    break;
  case 'N':
    if (memEQ(name, "XmlValue::DURATIO", 17)) {
    /*                                N      */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::DURATION;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    if (memEQ(name, "XmlValue::NOTATIO", 17)) {
    /*                                N      */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::NOTATION;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'Q':
    if (memEQ(name, "XmlIndexLookup::E", 17)) {
    /*                                Q      */
      {
        	DbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::EQ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "DbXml::LEVEL_ERRO", 17)) {
    /*                                R      */
      *iv_return = (IV)DbXml::LEVEL_ERROR;
      return PERL_constant_ISUV;
    }
    break;
  case 'T':
    if (memEQ(name, "XmlIndexLookup::G", 17)) {
    /*                                T      */
      {
        	DbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::GT;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    if (memEQ(name, "XmlIndexLookup::L", 17)) {
    /*                                T      */
      {
        	DbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::LT;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_19 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     DBXML_VERSION_MAJOR DBXML_VERSION_MINOR DBXML_VERSION_PATCH
     "DbXml::CATEGORY_ALL" "DbXml::DBXML_CHKSUM" "XmlEventType::CDATA"
     "XmlIndexLookup::GTE" "XmlIndexLookup::LTE" "XmlValue::DATE_TIME"
     "XmlValue::TEXT_NODE" */
  /* Offset 16 gives the best switch position.  */
  switch (name[16]) {
  case 'A':
    if (memEQ(name, "DbXml::CATEGORY_ALL", 19)) {
    /*                               ^         */
      *iv_return = (IV)DbXml::CATEGORY_ALL;
      return PERL_constant_ISUV;
    }
    if (memEQ(name, "XmlEventType::CDATA", 19)) {
    /*                               ^         */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::CDATA;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "XmlIndexLookup::GTE", 19)) {
    /*                               ^         */
      {
        	DbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::GTE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "XmlValue::DATE_TIME", 19)) {
    /*                               ^         */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::DATE_TIME;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'J':
    if (memEQ(name, "DBXML_VERSION_MAJOR", 19)) {
    /*                               ^         */
      *iv_return = (IV)DBXML_VERSION_MAJOR;
      return PERL_constant_ISUV;
    }
    break;
  case 'L':
    if (memEQ(name, "XmlIndexLookup::LTE", 19)) {
    /*                               ^         */
      {
        	DbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::LTE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "DBXML_VERSION_MINOR", 19)) {
    /*                               ^         */
      *iv_return = (IV)DBXML_VERSION_MINOR;
      return PERL_constant_ISUV;
    }
    break;
  case 'O':
    if (memEQ(name, "XmlValue::TEXT_NODE", 19)) {
    /*                               ^         */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::TEXT_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "DbXml::DBXML_CHKSUM", 19)) {
    /*                               ^         */
      *iv_return = (IV)DbXml::DBXML_CHKSUM;
      return PERL_constant_ISUV;
    }
    break;
  case 'T':
    if (memEQ(name, "DBXML_VERSION_PATCH", 19)) {
    /*                               ^         */
      *iv_return = (IV)DBXML_VERSION_PATCH;
      return PERL_constant_ISUV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_20 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::CATEGORY_NONE" "DbXml::DBXML_ENCRYPT" "DbXml::LEVEL_WARNING"
     "XmlIndexLookup::NONE" "XmlValue::HEX_BINARY" */
  /* Offset 8 gives the best switch position.  */
  switch (name[8]) {
  case ':':
    if (memEQ(name, "XmlValue::HEX_BINARY", 20)) {
    /*                       ^                  */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::HEX_BINARY;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'A':
    if (memEQ(name, "DbXml::CATEGORY_NONE", 20)) {
    /*                       ^                  */
      *iv_return = (IV)DbXml::CATEGORY_NONE;
      return PERL_constant_ISUV;
    }
    break;
  case 'B':
    if (memEQ(name, "DbXml::DBXML_ENCRYPT", 20)) {
    /*                       ^                  */
      *iv_return = (IV)DbXml::DBXML_ENCRYPT;
      return PERL_constant_ISUV;
    }
    break;
  case 'E':
    if (memEQ(name, "DbXml::LEVEL_WARNING", 20)) {
    /*                       ^                  */
      *iv_return = (IV)DbXml::LEVEL_WARNING;
      return PERL_constant_ISUV;
    }
    break;
  case 'L':
    if (memEQ(name, "XmlIndexLookup::NONE", 20)) {
    /*                       ^                  */
      {
        	DbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::NONE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_21 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::CATEGORY_QUERY" "DbXml::DBXML_GEN_NAME" "XmlEventType::Comment"
     "XmlQueryContext::Lazy" "XmlValue::ENTITY_NODE" "XmlValue::G_MONTH_DAY" */
  /* Offset 18 gives the best switch position.  */
  switch (name[18]) {
  case 'A':
    if (memEQ(name, "DbXml::DBXML_GEN_NAME", 21)) {
    /*                                 ^         */
      *iv_return = (IV)DbXml::DBXML_GEN_NAME;
      return PERL_constant_ISUV;
    }
    break;
  case 'D':
    if (memEQ(name, "XmlValue::G_MONTH_DAY", 21)) {
    /*                                 ^         */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::G_MONTH_DAY;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "DbXml::CATEGORY_QUERY", 21)) {
    /*                                 ^         */
      *iv_return = (IV)DbXml::CATEGORY_QUERY;
      return PERL_constant_ISUV;
    }
    break;
  case 'O':
    if (memEQ(name, "XmlValue::ENTITY_NODE", 21)) {
    /*                                 ^         */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::ENTITY_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'a':
    if (memEQ(name, "XmlQueryContext::Lazy", 21)) {
    /*                                 ^         */
      {
        	DbXml::XmlQueryContext::EvaluationType tmp = XmlQueryContext::Lazy;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'e':
    if (memEQ(name, "XmlEventType::Comment", 21)) {
    /*                                 ^         */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::Comment;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_22 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::DBXML_LAZY_DOCS" "XmlContainerConfig::On" "XmlQueryContext::Eager"
     "XmlValue::COMMENT_NODE" "XmlValue::ELEMENT_NODE" "XmlValue::G_YEAR_MONTH"
     */
  /* Offset 12 gives the best switch position.  */
  switch (name[12]) {
  case 'C':
    if (memEQ(name, "XmlContainerConfig::On", 22)) {
    /*                           ^                */
      {
        	DbXml::XmlContainerConfig::ConfigState tmp = XmlContainerConfig::On ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "XmlValue::ELEMENT_NODE", 22)) {
    /*                           ^                */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::ELEMENT_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "XmlValue::COMMENT_NODE", 22)) {
    /*                           ^                */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::COMMENT_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "XmlValue::G_YEAR_MONTH", 22)) {
    /*                           ^                */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::G_YEAR_MONTH;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "DbXml::DBXML_LAZY_DOCS", 22)) {
    /*                           ^                */
      *iv_return = (IV)DbXml::DBXML_LAZY_DOCS;
      return PERL_constant_ISUV;
    }
    break;
  case 'e':
    if (memEQ(name, "XmlQueryContext::Eager", 22)) {
    /*                           ^                */
      {
        	DbXml::XmlQueryContext::EvaluationType tmp = XmlQueryContext::Eager;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_23 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::CATEGORY_INDEXER" "DbXml::CATEGORY_MANAGER"
     "DbXml::DBXML_STATISTICS" "XmlContainerConfig::Off"
     "XmlValue::DOCUMENT_NODE" "XmlValue::NOTATION_NODE" */
  /* Offset 16 gives the best switch position.  */
  switch (name[16]) {
  case 'I':
    if (memEQ(name, "DbXml::CATEGORY_INDEXER", 23)) {
    /*                               ^             */
      *iv_return = (IV)DbXml::CATEGORY_INDEXER;
      return PERL_constant_ISUV;
    }
    break;
  case 'M':
    if (memEQ(name, "DbXml::CATEGORY_MANAGER", 23)) {
    /*                               ^             */
      *iv_return = (IV)DbXml::CATEGORY_MANAGER;
      return PERL_constant_ISUV;
    }
    break;
  case 'N':
    if (memEQ(name, "XmlValue::DOCUMENT_NODE", 23)) {
    /*                               ^             */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::DOCUMENT_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "XmlValue::NOTATION_NODE", 23)) {
    /*                               ^             */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::NOTATION_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "DbXml::DBXML_STATISTICS", 23)) {
    /*                               ^             */
      *iv_return = (IV)DbXml::DBXML_STATISTICS;
      return PERL_constant_ISUV;
    }
    break;
  case 'i':
    if (memEQ(name, "XmlContainerConfig::Off", 23)) {
    /*                               ^             */
      {
        	DbXml::XmlContainerConfig::ConfigState tmp = XmlContainerConfig::Off ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_24 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::DBXML_ADOPT_DBENV" "DbXml::DBXML_INDEX_NODES"
     "XmlEventType::Characters" "XmlEventType::EndElement"
     "XmlEventType::Whitespace" "XmlValue::ATTRIBUTE_NODE"
     "XmlValue::BASE_64_BINARY" "XmlValue::UNTYPED_ATOMIC" */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'C':
    if (memEQ(name, "XmlEventType::Characters", 24)) {
    /*                             ^                */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::Characters;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "DbXml::DBXML_ADOPT_DBENV", 24)) {
    /*                             ^                */
      *iv_return = (IV)DbXml::DBXML_ADOPT_DBENV;
      return PERL_constant_ISUV;
    }
    break;
  case 'E':
    if (memEQ(name, "XmlEventType::EndElement", 24)) {
    /*                             ^                */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::EndElement;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "XmlValue::ATTRIBUTE_NODE", 24)) {
    /*                             ^                */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::ATTRIBUTE_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "DbXml::DBXML_INDEX_NODES", 24)) {
    /*                             ^                */
      *iv_return = (IV)DbXml::DBXML_INDEX_NODES;
      return PERL_constant_ISUV;
    }
    break;
  case 'P':
    if (memEQ(name, "XmlValue::UNTYPED_ATOMIC", 24)) {
    /*                             ^                */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::UNTYPED_ATOMIC;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "XmlEventType::Whitespace", 24)) {
    /*                             ^                */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::Whitespace;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "XmlValue::BASE_64_BINARY", 24)) {
    /*                             ^                */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::BASE_64_BINARY;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_25 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::CATEGORY_CONTAINER" "DbXml::CATEGORY_NODESTORE"
     "DbXml::CATEGORY_OPTIMIZER" "DbXml::DBXML_INDEX_VALUES"
     "XmlEventType::EndDocument" "XmlException::EVENT_ERROR"
     "XmlValue::ANY_SIMPLE_TYPE" */
  /* Offset 22 gives the best switch position.  */
  switch (name[22]) {
  case 'N':
    if (memEQ(name, "DbXml::CATEGORY_CONTAINER", 25)) {
    /*                                     ^         */
      *iv_return = (IV)DbXml::CATEGORY_CONTAINER;
      return PERL_constant_ISUV;
    }
    break;
  case 'O':
    if (memEQ(name, "DbXml::CATEGORY_NODESTORE", 25)) {
    /*                                     ^         */
      *iv_return = (IV)DbXml::CATEGORY_NODESTORE;
      return PERL_constant_ISUV;
    }
    break;
  case 'R':
    if (memEQ(name, "XmlException::EVENT_ERROR", 25)) {
    /*                                     ^         */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::EVENT_ERROR ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "DbXml::DBXML_INDEX_VALUES", 25)) {
    /*                                     ^         */
      *iv_return = (IV)DbXml::DBXML_INDEX_VALUES;
      return PERL_constant_ISUV;
    }
    break;
  case 'Y':
    if (memEQ(name, "XmlValue::ANY_SIMPLE_TYPE", 25)) {
    /*                                     ^         */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::ANY_SIMPLE_TYPE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'Z':
    if (memEQ(name, "DbXml::CATEGORY_OPTIMIZER", 25)) {
    /*                                     ^         */
      *iv_return = (IV)DbXml::CATEGORY_OPTIMIZER;
      return PERL_constant_ISUV;
    }
    break;
  case 'e':
    if (memEQ(name, "XmlEventType::EndDocument", 25)) {
    /*                                     ^         */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::EndDocument;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_26 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::CATEGORY_DICTIONARY" "DbXml::DBXML_NO_STATISTICS"
     "DbXml::DBXML_REVERSE_ORDER" "DbXml::DBXML_TRANSACTIONAL"
     "XmlEventType::StartElement" "XmlException::NULL_POINTER"
     "XmlException::UNIQUE_ERROR" */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'E':
    if (memEQ(name, "DbXml::DBXML_REVERSE_ORDER", 26)) {
    /*                             ^                  */
      *iv_return = (IV)DbXml::DBXML_REVERSE_ORDER;
      return PERL_constant_ISUV;
    }
    break;
  case 'N':
    if (memEQ(name, "XmlException::NULL_POINTER", 26)) {
    /*                             ^                  */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::NULL_POINTER ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "DbXml::DBXML_NO_STATISTICS", 26)) {
    /*                             ^                  */
      *iv_return = (IV)DbXml::DBXML_NO_STATISTICS;
      return PERL_constant_ISUV;
    }
    break;
  case 'R':
    if (memEQ(name, "DbXml::DBXML_TRANSACTIONAL", 26)) {
    /*                             ^                  */
      *iv_return = (IV)DbXml::DBXML_TRANSACTIONAL;
      return PERL_constant_ISUV;
    }
    break;
  case 'S':
    if (memEQ(name, "XmlEventType::StartElement", 26)) {
    /*                             ^                  */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::StartElement;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "XmlException::UNIQUE_ERROR", 26)) {
    /*                             ^                  */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::UNIQUE_ERROR ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "DbXml::CATEGORY_DICTIONARY", 26)) {
    /*                             ^                  */
      *iv_return = (IV)DbXml::CATEGORY_DICTIONARY;
      return PERL_constant_ISUV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_27 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::DBXML_NO_AUTO_COMMIT" "DbXml::DBXML_NO_INDEX_NODES"
     "XmlContainer::NodeContainer" "XmlEventType::StartDocument"
     "XmlException::INVALID_VALUE" "XmlException::UNKNOWN_INDEX"
     "XmlQueryContext::LiveValues" "XmlValue::DAY_TIME_DURATION" */
  /* Offset 16 gives the best switch position.  */
  switch (name[16]) {
  case ':':
    if (memEQ(name, "XmlQueryContext::LiveValues", 27)) {
    /*                               ^                 */
      {
        	DbXml::XmlQueryContext::ReturnType tmp = XmlQueryContext::LiveValues;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'A':
    if (memEQ(name, "DbXml::DBXML_NO_AUTO_COMMIT", 27)) {
    /*                               ^                 */
      *iv_return = (IV)DbXml::DBXML_NO_AUTO_COMMIT;
      return PERL_constant_ISUV;
    }
    break;
  case 'I':
    if (memEQ(name, "DbXml::DBXML_NO_INDEX_NODES", 27)) {
    /*                               ^                 */
      *iv_return = (IV)DbXml::DBXML_NO_INDEX_NODES;
      return PERL_constant_ISUV;
    }
    break;
  case 'K':
    if (memEQ(name, "XmlException::UNKNOWN_INDEX", 27)) {
    /*                               ^                 */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::UNKNOWN_INDEX ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "XmlValue::DAY_TIME_DURATION", 27)) {
    /*                               ^                 */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::DAY_TIME_DURATION;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'V':
    if (memEQ(name, "XmlException::INVALID_VALUE", 27)) {
    /*                               ^                 */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::INVALID_VALUE ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'a':
    if (memEQ(name, "XmlEventType::StartDocument", 27)) {
    /*                               ^                 */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::StartDocument;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'd':
    if (memEQ(name, "XmlContainer::NodeContainer", 27)) {
    /*                               ^                 */
      {
        	DbXml::XmlContainer::ContainerType tmp = XmlContainer::NodeContainer ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_28 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::DBXML_ALLOW_AUTO_OPEN" "XmlException::CONTAINER_OPEN"
     "XmlException::DATABASE_ERROR" "XmlException::INTERNAL_ERROR"
     "XmlValue::CDATA_SECTION_NODE" "XmlValue::DOCUMENT_TYPE_NODE" */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'A':
    if (memEQ(name, "XmlValue::CDATA_SECTION_NODE", 28)) {
    /*                             ^                    */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::CDATA_SECTION_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "XmlException::CONTAINER_OPEN", 28)) {
    /*                             ^                    */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::CONTAINER_OPEN ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "XmlException::DATABASE_ERROR", 28)) {
    /*                             ^                    */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::DATABASE_ERROR ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "XmlException::INTERNAL_ERROR", 28)) {
    /*                             ^                    */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::INTERNAL_ERROR ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "DbXml::DBXML_ALLOW_AUTO_OPEN", 28)) {
    /*                             ^                    */
      *iv_return = (IV)DbXml::DBXML_ALLOW_AUTO_OPEN;
      return PERL_constant_ISUV;
    }
    break;
  case 'M':
    if (memEQ(name, "XmlValue::DOCUMENT_TYPE_NODE", 28)) {
    /*                             ^                    */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::DOCUMENT_TYPE_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_29 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::DBXML_ALLOW_VALIDATION" "XmlException::LAZY_EVALUATION"
     "XmlException::NO_MEMORY_ERROR" "XmlValue::YEAR_MONTH_DURATION" */
  /* Offset 17 gives the best switch position.  */
  switch (name[17]) {
  case 'M':
    if (memEQ(name, "XmlException::NO_MEMORY_ERROR", 29)) {
    /*                                ^                  */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::NO_MEMORY_ERROR ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "XmlValue::YEAR_MONTH_DURATION", 29)) {
    /*                                ^                  */
      {
        	DbXml::XmlValue::Type tmp = DbXml::XmlValue::YEAR_MONTH_DURATION;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "DbXml::DBXML_ALLOW_VALIDATION", 29)) {
    /*                                ^                  */
      *iv_return = (IV)DbXml::DBXML_ALLOW_VALIDATION;
      return PERL_constant_ISUV;
    }
    break;
  case 'Y':
    if (memEQ(name, "XmlException::LAZY_EVALUATION", 29)) {
    /*                                ^                  */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::LAZY_EVALUATION ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_30 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "XmlContainerConfig::UseDefault" "XmlException::CONTAINER_CLOSED"
     "XmlException::CONTAINER_EXISTS" "XmlException::VERSION_MISMATCH" */
  /* Offset 25 gives the best switch position.  */
  switch (name[25]) {
  case 'L':
    if (memEQ(name, "XmlException::CONTAINER_CLOSED", 30)) {
    /*                                        ^           */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::CONTAINER_CLOSED ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "XmlException::VERSION_MISMATCH", 30)) {
    /*                                        ^           */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::VERSION_MISMATCH ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'X':
    if (memEQ(name, "XmlException::CONTAINER_EXISTS", 30)) {
    /*                                        ^           */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::CONTAINER_EXISTS ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'f':
    if (memEQ(name, "XmlContainerConfig::UseDefault", 30)) {
    /*                                        ^           */
      {
        	DbXml::XmlContainerConfig::ConfigState tmp = XmlContainerConfig::UseDefault ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_31 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "XmlContainer::WholedocContainer" "XmlException::TRANSACTION_ERROR"
     "XmlIndexSpecification::KEY_NONE" "XmlValue::ENTITY_REFERENCE_NODE" */
  /* Offset 5 gives the best switch position.  */
  switch (name[5]) {
  case 'c':
    if (memEQ(name, "XmlException::TRANSACTION_ERROR", 31)) {
    /*                    ^                                */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::TRANSACTION_ERROR ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'd':
    if (memEQ(name, "XmlIndexSpecification::KEY_NONE", 31)) {
    /*                    ^                                */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::KEY_NONE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'l':
    if (memEQ(name, "XmlValue::ENTITY_REFERENCE_NODE", 31)) {
    /*                    ^                                */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::ENTITY_REFERENCE_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'n':
    if (memEQ(name, "XmlContainer::WholedocContainer", 31)) {
    /*                    ^                                */
      {
        	DbXml::XmlContainer::ContainerType tmp = XmlContainer::WholedocContainer ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_32 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::DBXML_DOCUMENT_PROJECTION" "XmlEventType::EndEntityReference"
     "XmlException::DOCUMENT_NOT_FOUND" "XmlException::QUERY_PARSER_ERROR"
     "XmlIndexSpecification::NODE_NONE" "XmlIndexSpecification::PATH_EDGE"
     "XmlIndexSpecification::PATH_NODE" "XmlIndexSpecification::PATH_NONE"
     "XmlIndexSpecification::UNIQUE_ON" "XmlValue::DOCUMENT_FRAGMENT_NODE" */
  /* Offset 25 gives the best switch position.  */
  switch (name[25]) {
  case 'D':
    if (memEQ(name, "XmlIndexSpecification::NODE_NONE", 32)) {
    /*                                        ^             */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::NODE_NONE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "XmlIndexSpecification::UNIQUE_ON", 32)) {
    /*                                        ^             */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::UNIQUE_ON;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'J':
    if (memEQ(name, "DbXml::DBXML_DOCUMENT_PROJECTION", 32)) {
    /*                                        ^             */
      *iv_return = (IV)DbXml::DBXML_DOCUMENT_PROJECTION;
      return PERL_constant_ISUV;
    }
    break;
  case 'N':
    if (memEQ(name, "XmlValue::DOCUMENT_FRAGMENT_NODE", 32)) {
    /*                                        ^             */
      {
        	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::DOCUMENT_FRAGMENT_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "XmlException::QUERY_PARSER_ERROR", 32)) {
    /*                                        ^             */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::QUERY_PARSER_ERROR ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "XmlException::DOCUMENT_NOT_FOUND", 32)) {
    /*                                        ^             */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::DOCUMENT_NOT_FOUND ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    if (memEQ(name, "XmlIndexSpecification::PATH_EDGE", 32)) {
    /*                                        ^             */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::PATH_EDGE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    if (memEQ(name, "XmlIndexSpecification::PATH_NODE", 32)) {
    /*                                        ^             */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::PATH_NODE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    if (memEQ(name, "XmlIndexSpecification::PATH_NONE", 32)) {
    /*                                        ^             */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::PATH_NONE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'f':
    if (memEQ(name, "XmlEventType::EndEntityReference", 32)) {
    /*                                        ^             */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::EndEntityReference;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_34 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "DbXml::DBXML_ALLOW_EXTERNAL_ACCESS" "XmlEventType::StartEntityReference"
     "XmlException::INDEXER_PARSER_ERROR" */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'I':
    if (memEQ(name, "XmlException::INDEXER_PARSER_ERROR", 34)) {
    /*                             ^                          */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::INDEXER_PARSER_ERROR ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "DbXml::DBXML_ALLOW_EXTERNAL_ACCESS", 34)) {
    /*                             ^                          */
      *iv_return = (IV)DbXml::DBXML_ALLOW_EXTERNAL_ACCESS;
      return PERL_constant_ISUV;
    }
    break;
  case 'S':
    if (memEQ(name, "XmlEventType::StartEntityReference", 34)) {
    /*                             ^                          */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::StartEntityReference;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_35 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "XmlEventType::ProcessingInstruction"
     "XmlIndexSpecification::KEY_EQUALITY"
     "XmlIndexSpecification::KEY_PRESENCE"
     "XmlIndexSpecification::NODE_ELEMENT" */
  /* Offset 30 gives the best switch position.  */
  switch (name[30]) {
  case 'A':
    if (memEQ(name, "XmlIndexSpecification::KEY_EQUALITY", 35)) {
    /*                                             ^           */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::KEY_EQUALITY;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "XmlIndexSpecification::NODE_ELEMENT", 35)) {
    /*                                             ^           */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::NODE_ELEMENT;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "XmlIndexSpecification::KEY_PRESENCE", 35)) {
    /*                                             ^           */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::KEY_PRESENCE;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'c':
    if (memEQ(name, "XmlEventType::ProcessingInstruction", 35)) {
    /*                                             ^           */
      {
        	DbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::ProcessingInstruction;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_36 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "XmlException::QUERY_EVALUATION_ERROR"
     "XmlIndexSpecification::KEY_SUBSTRING"
     "XmlIndexSpecification::NODE_METADATA" */
  /* Offset 23 gives the best switch position.  */
  switch (name[23]) {
  case 'K':
    if (memEQ(name, "XmlIndexSpecification::KEY_SUBSTRING", 36)) {
    /*                                      ^                   */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::KEY_SUBSTRING;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "XmlException::QUERY_EVALUATION_ERROR", 36)) {
    /*                                      ^                   */
      {
        	DbXml::XmlException::ExceptionCode tmp = XmlException::QUERY_EVALUATION_ERROR ;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "XmlIndexSpecification::NODE_METADATA", 36)) {
    /*                                      ^                   */
      {
        	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::NODE_METADATA;
        *iv_return = (IV)(UV)tmp;
        return PERL_constant_ISUV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant (pTHX_ const char *name, STRLEN len, IV *iv_return) {
  /* Initially switch on the length of the name.  */
  /* When generated this function returned values for the list of names given
     in this section of perl code.  Rather than manually editing these functions
     to add or remove constants, which would result in this comment and section
     of code becoming inaccurate, we recommend that you edit this section of
     code, and use it to regenerate a new set of constant functions which you
     then use to replace the originals.

     Regenerate these constant functions by feeding this entire source file to
     perl -x

#!/usr/bin/perl -w
use ExtUtils::Constant qw (constant_types C_constant XS_constant);

my $types = {map {($_, 1)} qw(UV)};
my @names = (qw(),
            {name=>"DBXML_VERSION_MAJOR", type=>"UV", macro=>"1"},
            {name=>"DBXML_VERSION_MINOR", type=>"UV", macro=>"1"},
            {name=>"DBXML_VERSION_PATCH", type=>"UV", macro=>"1"},
            {name=>"DbXml::CATEGORY_ALL", type=>"UV", macro=>"1"},
            {name=>"DbXml::CATEGORY_CONTAINER", type=>"UV", macro=>"1"},
            {name=>"DbXml::CATEGORY_DICTIONARY", type=>"UV", macro=>"1"},
            {name=>"DbXml::CATEGORY_INDEXER", type=>"UV", macro=>"1"},
            {name=>"DbXml::CATEGORY_MANAGER", type=>"UV", macro=>"1"},
            {name=>"DbXml::CATEGORY_NODESTORE", type=>"UV", macro=>"1"},
            {name=>"DbXml::CATEGORY_NONE", type=>"UV", macro=>"1"},
            {name=>"DbXml::CATEGORY_OPTIMIZER", type=>"UV", macro=>"1"},
            {name=>"DbXml::CATEGORY_QUERY", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_ADOPT_DBENV", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_ALLOW_AUTO_OPEN", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_ALLOW_EXTERNAL_ACCESS", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_ALLOW_VALIDATION", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_CHKSUM", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_DOCUMENT_PROJECTION", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_ENCRYPT", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_GEN_NAME", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_INDEX_NODES", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_INDEX_VALUES", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_LAZY_DOCS", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_NO_AUTO_COMMIT", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_NO_INDEX_NODES", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_NO_STATISTICS", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_REVERSE_ORDER", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_STATISTICS", type=>"UV", macro=>"1"},
            {name=>"DbXml::DBXML_TRANSACTIONAL", type=>"UV", macro=>"1"},
            {name=>"DbXml::LEVEL_ALL", type=>"UV", macro=>"1"},
            {name=>"DbXml::LEVEL_DEBUG", type=>"UV", macro=>"1"},
            {name=>"DbXml::LEVEL_ERROR", type=>"UV", macro=>"1"},
            {name=>"DbXml::LEVEL_INFO", type=>"UV", macro=>"1"},
            {name=>"DbXml::LEVEL_NONE", type=>"UV", macro=>"1"},
            {name=>"DbXml::LEVEL_WARNING", type=>"UV", macro=>"1"},
            {name=>"XmlContainer::NodeContainer", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlContainer::ContainerType tmp = XmlContainer::NodeContainer ;"},
            {name=>"XmlContainer::WholedocContainer", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlContainer::ContainerType tmp = XmlContainer::WholedocContainer ;"},
            {name=>"XmlContainerConfig::Off", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlContainerConfig::ConfigState tmp = XmlContainerConfig::Off ;"},
            {name=>"XmlContainerConfig::On", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlContainerConfig::ConfigState tmp = XmlContainerConfig::On ;"},
            {name=>"XmlContainerConfig::UseDefault", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlContainerConfig::ConfigState tmp = XmlContainerConfig::UseDefault ;"},
            {name=>"XmlEventType::CDATA", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::CDATA;"},
            {name=>"XmlEventType::Characters", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::Characters;"},
            {name=>"XmlEventType::Comment", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::Comment;"},
            {name=>"XmlEventType::DTD", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::DTD;"},
            {name=>"XmlEventType::EndDocument", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::EndDocument;"},
            {name=>"XmlEventType::EndElement", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::EndElement;"},
            {name=>"XmlEventType::EndEntityReference", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::EndEntityReference;"},
            {name=>"XmlEventType::ProcessingInstruction", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::ProcessingInstruction;"},
            {name=>"XmlEventType::StartDocument", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::StartDocument;"},
            {name=>"XmlEventType::StartElement", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::StartElement;"},
            {name=>"XmlEventType::StartEntityReference", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::StartEntityReference;"},
            {name=>"XmlEventType::Whitespace", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlEventReader::XmlEventType tmp = XmlEventReader::Whitespace;"},
            {name=>"XmlException::CONTAINER_CLOSED", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::CONTAINER_CLOSED ;"},
            {name=>"XmlException::CONTAINER_EXISTS", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::CONTAINER_EXISTS ;"},
            {name=>"XmlException::CONTAINER_NOT_FOUND", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::CONTAINER_NOT_FOUND ;"},
            {name=>"XmlException::CONTAINER_OPEN", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::CONTAINER_OPEN ;"},
            {name=>"XmlException::DATABASE_ERROR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::DATABASE_ERROR ;"},
            {name=>"XmlException::DOCUMENT_NOT_FOUND", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::DOCUMENT_NOT_FOUND ;"},
            {name=>"XmlException::EVENT_ERROR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::EVENT_ERROR ;"},
            {name=>"XmlException::INDEXER_PARSER_ERROR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::INDEXER_PARSER_ERROR ;"},
            {name=>"XmlException::INTERNAL_ERROR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::INTERNAL_ERROR ;"},
            {name=>"XmlException::INVALID_VALUE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::INVALID_VALUE ;"},
            {name=>"XmlException::LAZY_EVALUATION", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::LAZY_EVALUATION ;"},
            {name=>"XmlException::NO_MEMORY_ERROR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::NO_MEMORY_ERROR ;"},
            {name=>"XmlException::NULL_POINTER", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::NULL_POINTER ;"},
            {name=>"XmlException::QUERY_EVALUATION_ERROR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::QUERY_EVALUATION_ERROR ;"},
            {name=>"XmlException::QUERY_PARSER_ERROR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::QUERY_PARSER_ERROR ;"},
            {name=>"XmlException::TRANSACTION_ERROR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::TRANSACTION_ERROR ;"},
            {name=>"XmlException::UNIQUE_ERROR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::UNIQUE_ERROR ;"},
            {name=>"XmlException::UNKNOWN_INDEX", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::UNKNOWN_INDEX ;"},
            {name=>"XmlException::VERSION_MISMATCH", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlException::ExceptionCode tmp = XmlException::VERSION_MISMATCH ;"},
            {name=>"XmlIndexLookup::EQ", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::EQ;"},
            {name=>"XmlIndexLookup::GT", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::GT;"},
            {name=>"XmlIndexLookup::GTE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::GTE;"},
            {name=>"XmlIndexLookup::LT", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::LT;"},
            {name=>"XmlIndexLookup::LTE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::LTE;"},
            {name=>"XmlIndexLookup::NONE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexLookup::Operation tmp = XmlIndexLookup::NONE;"},
            {name=>"XmlIndexSpecification::KEY_EQUALITY", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::KEY_EQUALITY;"},
            {name=>"XmlIndexSpecification::KEY_NONE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::KEY_NONE;"},
            {name=>"XmlIndexSpecification::KEY_PRESENCE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::KEY_PRESENCE;"},
            {name=>"XmlIndexSpecification::KEY_SUBSTRING", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::KEY_SUBSTRING;"},
            {name=>"XmlIndexSpecification::NODE_ATTRIBUTE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::NODE_ATTRIBUTE;"},
            {name=>"XmlIndexSpecification::NODE_ELEMENT", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::NODE_ELEMENT;"},
            {name=>"XmlIndexSpecification::NODE_METADATA", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::NODE_METADATA;"},
            {name=>"XmlIndexSpecification::NODE_NONE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::NODE_NONE;"},
            {name=>"XmlIndexSpecification::PATH_EDGE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::PATH_EDGE;"},
            {name=>"XmlIndexSpecification::PATH_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::PATH_NODE;"},
            {name=>"XmlIndexSpecification::PATH_NONE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::PATH_NONE;"},
            {name=>"XmlIndexSpecification::UNIQUE_OFF", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::UNIQUE_OFF;"},
            {name=>"XmlIndexSpecification::UNIQUE_ON", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::UNIQUE_ON;"},
            {name=>"XmlQueryContext::Eager", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlQueryContext::EvaluationType tmp = XmlQueryContext::Eager;"},
            {name=>"XmlQueryContext::Lazy", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlQueryContext::EvaluationType tmp = XmlQueryContext::Lazy;"},
            {name=>"XmlQueryContext::LiveValues", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlQueryContext::ReturnType tmp = XmlQueryContext::LiveValues;"},
            {name=>"XmlValue::ANY_SIMPLE_TYPE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::ANY_SIMPLE_TYPE;"},
            {name=>"XmlValue::ANY_URI", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::ANY_URI;"},
            {name=>"XmlValue::ATTRIBUTE_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::ATTRIBUTE_NODE;"},
            {name=>"XmlValue::BASE_64_BINARY", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::BASE_64_BINARY;"},
            {name=>"XmlValue::BINARY", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::BINARY;"},
            {name=>"XmlValue::BOOLEAN", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::BOOLEAN;"},
            {name=>"XmlValue::CDATA_SECTION_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::CDATA_SECTION_NODE;"},
            {name=>"XmlValue::COMMENT_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::COMMENT_NODE;"},
            {name=>"XmlValue::DATE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::DATE;"},
            {name=>"XmlValue::DATE_TIME", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::DATE_TIME;"},
            {name=>"XmlValue::DAY_TIME_DURATION", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::DAY_TIME_DURATION;"},
            {name=>"XmlValue::DECIMAL", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::DECIMAL;"},
            {name=>"XmlValue::DOCUMENT_FRAGMENT_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::DOCUMENT_FRAGMENT_NODE;"},
            {name=>"XmlValue::DOCUMENT_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::DOCUMENT_NODE;"},
            {name=>"XmlValue::DOCUMENT_TYPE_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::DOCUMENT_TYPE_NODE;"},
            {name=>"XmlValue::DOUBLE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::DOUBLE;"},
            {name=>"XmlValue::DURATION", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::DURATION;"},
            {name=>"XmlValue::ELEMENT_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::ELEMENT_NODE;"},
            {name=>"XmlValue::ENTITY_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::ENTITY_NODE;"},
            {name=>"XmlValue::ENTITY_REFERENCE_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::ENTITY_REFERENCE_NODE;"},
            {name=>"XmlValue::FLOAT", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::FLOAT;"},
            {name=>"XmlValue::G_DAY", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::G_DAY;"},
            {name=>"XmlValue::G_MONTH", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::G_MONTH;"},
            {name=>"XmlValue::G_MONTH_DAY", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::G_MONTH_DAY;"},
            {name=>"XmlValue::G_YEAR", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::G_YEAR;"},
            {name=>"XmlValue::G_YEAR_MONTH", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::G_YEAR_MONTH;"},
            {name=>"XmlValue::HEX_BINARY", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::HEX_BINARY;"},
            {name=>"XmlValue::NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::NODE;"},
            {name=>"XmlValue::NONE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::NONE;"},
            {name=>"XmlValue::NOTATION", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::NOTATION;"},
            {name=>"XmlValue::NOTATION_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::NOTATION_NODE;"},
            {name=>"XmlValue::PROCESSING_INSTRUCTION_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::PROCESSING_INSTRUCTION_NODE;"},
            {name=>"XmlValue::QNAME", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::QNAME;"},
            {name=>"XmlValue::STRING", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::STRING;"},
            {name=>"XmlValue::TEXT_NODE", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::NodeType tmp = DbXml::XmlValue::TEXT_NODE;"},
            {name=>"XmlValue::TIME", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::TIME;"},
            {name=>"XmlValue::UNTYPED_ATOMIC", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::UNTYPED_ATOMIC;"},
            {name=>"XmlValue::YEAR_MONTH_DURATION", type=>"UV", macro=>"1", value=>"(UV)tmp", pre=>"\tDbXml::XmlValue::Type tmp = DbXml::XmlValue::YEAR_MONTH_DURATION;"});

print constant_types(); # macro defs
foreach (C_constant ("DbXml", 'constant', 'UV', $types, undef, 3, @names) ) {
    print $_, "\n"; # C constant subs
}
print "#### XS Section:\n";
print XS_constant ("DbXml", $types);
__END__
   */

  switch (len) {
  case 14:
    return constant_14 (aTHX_ name, iv_return);
    break;
  case 15:
    return constant_15 (aTHX_ name, iv_return);
    break;
  case 16:
    return constant_16 (aTHX_ name, iv_return);
    break;
  case 17:
    return constant_17 (aTHX_ name, iv_return);
    break;
  case 18:
    return constant_18 (aTHX_ name, iv_return);
    break;
  case 19:
    return constant_19 (aTHX_ name, iv_return);
    break;
  case 20:
    return constant_20 (aTHX_ name, iv_return);
    break;
  case 21:
    return constant_21 (aTHX_ name, iv_return);
    break;
  case 22:
    return constant_22 (aTHX_ name, iv_return);
    break;
  case 23:
    return constant_23 (aTHX_ name, iv_return);
    break;
  case 24:
    return constant_24 (aTHX_ name, iv_return);
    break;
  case 25:
    return constant_25 (aTHX_ name, iv_return);
    break;
  case 26:
    return constant_26 (aTHX_ name, iv_return);
    break;
  case 27:
    return constant_27 (aTHX_ name, iv_return);
    break;
  case 28:
    return constant_28 (aTHX_ name, iv_return);
    break;
  case 29:
    return constant_29 (aTHX_ name, iv_return);
    break;
  case 30:
    return constant_30 (aTHX_ name, iv_return);
    break;
  case 31:
    return constant_31 (aTHX_ name, iv_return);
    break;
  case 32:
    return constant_32 (aTHX_ name, iv_return);
    break;
  case 33:
    /* Names all of length 33.  */
    /* "XmlException::CONTAINER_NOT_FOUND" "XmlIndexSpecification::UNIQUE_OFF"
       */
    /* Offset 5 gives the best switch position.  */
    switch (name[5]) {
    case 'c':
      if (memEQ(name, "XmlException::CONTAINER_NOT_FOUND", 33)) {
      /*                    ^                                  */
        {
          	DbXml::XmlException::ExceptionCode tmp = XmlException::CONTAINER_NOT_FOUND ;
          *iv_return = (IV)(UV)tmp;
          return PERL_constant_ISUV;
        }
      }
      break;
    case 'd':
      if (memEQ(name, "XmlIndexSpecification::UNIQUE_OFF", 33)) {
      /*                    ^                                  */
        {
          	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::UNIQUE_OFF;
          *iv_return = (IV)(UV)tmp;
          return PERL_constant_ISUV;
        }
      }
      break;
    }
    break;
  case 34:
    return constant_34 (aTHX_ name, iv_return);
    break;
  case 35:
    return constant_35 (aTHX_ name, iv_return);
    break;
  case 36:
    return constant_36 (aTHX_ name, iv_return);
    break;
  case 37:
    /* Names all of length 37.  */
    /* "XmlIndexSpecification::NODE_ATTRIBUTE"
       "XmlValue::PROCESSING_INSTRUCTION_NODE" */
    /* Offset 31 gives the best switch position.  */
    switch (name[31]) {
    case 'N':
      if (memEQ(name, "XmlValue::PROCESSING_INSTRUCTION_NODE", 37)) {
      /*                                              ^            */
        {
          	DbXml::XmlValue::NodeType tmp = DbXml::XmlValue::PROCESSING_INSTRUCTION_NODE;
          *iv_return = (IV)(UV)tmp;
          return PERL_constant_ISUV;
        }
      }
      break;
    case 'R':
      if (memEQ(name, "XmlIndexSpecification::NODE_ATTRIBUTE", 37)) {
      /*                                              ^            */
        {
          	DbXml::XmlIndexSpecification::Type tmp = XmlIndexSpecification::NODE_ATTRIBUTE;
          *iv_return = (IV)(UV)tmp;
          return PERL_constant_ISUV;
        }
      }
      break;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

