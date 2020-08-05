/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     MYEOF = 0,
     _LANG_XPATH2_ = 258,
     _LANG_XQUERY_ = 259,
     _LANG_XQUERY_FULLTEXT_ = 260,
     _LANG_XQUERY_UPDATE_ = 261,
     _LANG_XQUERY_FULLTEXT_UPDATE_ = 262,
     _LANG_XSLT2_ = 263,
     _DOLLAR_ = 264,
     _COLON_EQUALS_ = 265,
     _ASTERISK_ = 266,
     _BAR_ = 267,
     _BANG_EQUALS_ = 268,
     _LESS_THAN_EQUALS_ = 269,
     _GREATER_THAN_ = 270,
     _GREATER_THAN_EQUALS_ = 271,
     _LESS_THAN_LESS_THAN_ = 272,
     _GREATER_THAN_GREATER_THAN_ = 273,
     _COLON_COLON_ = 274,
     _AT_SIGN_ = 275,
     _LSQUARE_ = 276,
     _RSQUARE_ = 277,
     _QUESTION_MARK_ = 278,
     _BAR_BAR_ = 279,
     _AMP_AMP_ = 280,
     _LESS_THAN_OP_OR_TAG_ = 281,
     _START_TAG_CLOSE_ = 282,
     _END_TAG_CLOSE_ = 283,
     _PI_START_ = 284,
     _XML_COMMENT_START_ = 285,
     _XML_COMMENT_END_ = 286,
     _SLASH_SLASH_ = 287,
     _END_TAG_OPEN_ = 288,
     _PRAGMA_OPEN_ = 289,
     _COMMA_ = 290,
     _PLUS_ = 291,
     _MINUS_ = 292,
     _SLASH_ = 293,
     _EQUALS_ = 294,
     _DOT_ = 295,
     _DOT_DOT_ = 296,
     _LPAR_ = 297,
     _RPAR_ = 298,
     _EMPTY_TAG_CLOSE_ = 299,
     _VALUE_INDICATOR_ = 300,
     _OPEN_QUOT_ = 301,
     _CLOSE_QUOT_ = 302,
     _OPEN_APOS_ = 303,
     _CLOSE_APOS_ = 304,
     _LBRACE_ = 305,
     _LBRACE_EXPR_ENCLOSURE_ = 306,
     _RBRACE_ = 307,
     _SEMICOLON_ = 308,
     _BANG_ = 309,
     _HASH_ = 310,
     _INTEGER_LITERAL_ = 311,
     _DECIMAL_LITERAL_ = 312,
     _DOUBLE_LITERAL_ = 313,
     _ATTRIBUTE_ = 314,
     _COMMENT_ = 315,
     _DOCUMENT_NODE_ = 316,
     _NODE_ = 317,
     _PROCESSING_INSTRUCTION_ = 318,
     _SCHEMA_ATTRIBUTE_ = 319,
     _SCHEMA_ELEMENT_ = 320,
     _TEXT_ = 321,
     _EMPTY_SEQUENCE_ = 322,
     _BOUNDARY_SPACE_ = 323,
     _FT_OPTION_ = 324,
     _BASE_URI_ = 325,
     _LAX_ = 326,
     _STRICT_ = 327,
     _IDIV_ = 328,
     _CHILD_ = 329,
     _DESCENDANT_ = 330,
     _SELF_ = 331,
     _DESCENDANT_OR_SELF_ = 332,
     _FOLLOWING_SIBLING_ = 333,
     _FOLLOWING_ = 334,
     _PARENT_ = 335,
     _ANCESTOR_ = 336,
     _PRECEDING_SIBLING_ = 337,
     _PRECEDING_ = 338,
     _ANCESTOR_OR_SELF_ = 339,
     _DOCUMENT_ = 340,
     _NOT_ = 341,
     _SENSITIVE_ = 342,
     _INSENSITIVE_ = 343,
     _DIACRITICS_ = 344,
     _WITHOUT_ = 345,
     _WITHOUT_C_ = 346,
     _STEMMING_ = 347,
     _THESAURUS_ = 348,
     _STOP_ = 349,
     _WILDCARDS_ = 350,
     _ENTIRE_ = 351,
     _CONTENT_ = 352,
     _WORD_ = 353,
     _TYPE_ = 354,
     _START_ = 355,
     _END_ = 356,
     _MOST_ = 357,
     _SKIP_ = 358,
     _COPY_ = 359,
     _VALUE_ = 360,
     _WHITESPACE_ = 361,
     _PI_CONTENT_ = 362,
     _XML_COMMENT_CONTENT_ = 363,
     _EQ_ = 364,
     _NE_ = 365,
     _LT_ = 366,
     _LE_ = 367,
     _GT_ = 368,
     _GE_ = 369,
     _AT_ = 370,
     _QUOT_ATTR_CONTENT_ = 371,
     _APOS_ATTR_CONTENT_ = 372,
     _WHITESPACE_ELEMENT_CONTENT_ = 373,
     _ELEMENT_CONTENT_ = 374,
     _AT_LM_ = 375,
     _ORDERED_ = 376,
     _UNORDERED_ = 377,
     _QNAME_ = 378,
     _XMLNS_QNAME_ = 379,
     _CONSTR_QNAME_ = 380,
     _STRING_LITERAL_ = 381,
     _VARIABLE_ = 382,
     _NCNAME_COLON_STAR_ = 383,
     _STAR_COLON_NCNAME_ = 384,
     _PI_TARGET_ = 385,
     _PRAGMA_CONTENT_ = 386,
     _RETURN_ = 387,
     _FOR_ = 388,
     _IN_ = 389,
     _LET_ = 390,
     _WHERE_ = 391,
     _BY_ = 392,
     _ORDER_ = 393,
     _STABLE_ = 394,
     _ASCENDING_ = 395,
     _DESCENDING_ = 396,
     _EMPTY_ = 397,
     _GREATEST_ = 398,
     _LEAST_ = 399,
     _COLLATION_ = 400,
     _SOME_ = 401,
     _EVERY_ = 402,
     _SATISFIES_ = 403,
     _TYPESWITCH_ = 404,
     _CASE_ = 405,
     _CASE_S_ = 406,
     _AS_ = 407,
     _IF_ = 408,
     _THEN_ = 409,
     _ELSE_ = 410,
     _OR_ = 411,
     _AND_ = 412,
     _INSTANCE_ = 413,
     _OF_ = 414,
     _CASTABLE_ = 415,
     _TO_ = 416,
     _DIV_ = 417,
     _MOD_ = 418,
     _UNION_ = 419,
     _INTERSECT_ = 420,
     _EXCEPT_ = 421,
     _VALIDATE_ = 422,
     _CAST_ = 423,
     _TREAT_ = 424,
     _IS_ = 425,
     _PRESERVE_ = 426,
     _STRIP_ = 427,
     _NAMESPACE_ = 428,
     _ITEM_ = 429,
     _EXTERNAL_ = 430,
     _ENCODING_ = 431,
     _NO_PRESERVE_ = 432,
     _INHERIT_ = 433,
     _NO_INHERIT_ = 434,
     _DECLARE_ = 435,
     _CONSTRUCTION_ = 436,
     _ORDERING_ = 437,
     _DEFAULT_ = 438,
     _COPY_NAMESPACES_ = 439,
     _OPTION_ = 440,
     _XQUERY_ = 441,
     _VERSION_ = 442,
     _IMPORT_ = 443,
     _SCHEMA_ = 444,
     _MODULE_ = 445,
     _ELEMENT_ = 446,
     _FUNCTION_ = 447,
     _FUNCTION_EXT_ = 448,
     _SCORE_ = 449,
     _FTCONTAINS_ = 450,
     _WEIGHT_ = 451,
     _WINDOW_ = 452,
     _DISTANCE_ = 453,
     _OCCURS_ = 454,
     _TIMES_ = 455,
     _SAME_ = 456,
     _DIFFERENT_ = 457,
     _LOWERCASE_ = 458,
     _UPPERCASE_ = 459,
     _RELATIONSHIP_ = 460,
     _LEVELS_ = 461,
     _LANGUAGE_ = 462,
     _ANY_ = 463,
     _ALL_ = 464,
     _PHRASE_ = 465,
     _EXACTLY_ = 466,
     _FROM_ = 467,
     _WORDS_ = 468,
     _SENTENCES_ = 469,
     _PARAGRAPHS_ = 470,
     _SENTENCE_ = 471,
     _PARAGRAPH_ = 472,
     _REPLACE_ = 473,
     _MODIFY_ = 474,
     _FIRST_ = 475,
     _INSERT_ = 476,
     _BEFORE_ = 477,
     _AFTER_ = 478,
     _REVALIDATION_ = 479,
     _WITH_ = 480,
     _WITH_FT_ = 481,
     _NODES_ = 482,
     _RENAME_ = 483,
     _LAST_ = 484,
     _DELETE_ = 485,
     _INTO_ = 486,
     _UPDATING_ = 487,
     _ID_ = 488,
     _KEY_ = 489,
     _TEMPLATE_ = 490,
     _MATCHES_ = 491,
     _NAME_ = 492,
     _CALL_ = 493,
     _APPLY_ = 494,
     _TEMPLATES_ = 495,
     _MODE_ = 496,
     _XSLT_END_ELEMENT_ = 497,
     _XSLT_STYLESHEET_ = 498,
     _XSLT_TEMPLATE_ = 499,
     _XSLT_VALUE_OF_ = 500,
     _XSLT_TEXT_ = 501,
     _XSLT_APPLY_TEMPLATES_ = 502,
     _XSLT_CALL_TEMPLATE_ = 503,
     _XSLT_WITH_PARAM_ = 504,
     _XSLT_SEQUENCE_ = 505,
     _XSLT_PARAM_ = 506,
     _XSLT_FUNCTION_ = 507,
     _XSLT_CHOOSE_ = 508,
     _XSLT_WHEN_ = 509,
     _XSLT_OTHERWISE_ = 510,
     _XSLT_IF_ = 511,
     _XSLT_VARIABLE_ = 512,
     _XSLT_COMMENT_ = 513,
     _XSLT_PI_ = 514,
     _XSLT_DOCUMENT_ = 515,
     _XSLT_ATTRIBUTE_ = 516,
     _XSLT_NAMESPACE_ = 517,
     _XSLT_ELEMENT_ = 518,
     _XSLT_ANALYZE_STRING_ = 519,
     _XSLT_MATCHING_SUBSTRING_ = 520,
     _XSLT_NON_MATCHING_SUBSTRING_ = 521,
     _XSLT_COPY_OF_ = 522,
     _XSLT_COPY_ = 523,
     _XSLT_FOR_EACH_ = 524,
     _XSLT_OUTPUT_ = 525,
     _XSLT_IMPORT_SCHEMA_ = 526,
     _XSLT_VERSION_ = 527,
     _XSLT_MODE_ = 528,
     _XSLT_NAME_ = 529,
     _XSLT_DOCTYPE_PUBLIC_ = 530,
     _XSLT_DOCTYPE_SYSTEM_ = 531,
     _XSLT_ENCODING_ = 532,
     _XSLT_MEDIA_TYPE_ = 533,
     _XSLT_NORMALIZATION_FORM_ = 534,
     _XSLT_STANDALONE_ = 535,
     _XSLT_EXCLUDE_RESULT_PREFIXES_ = 536,
     _XSLT_NAMESPACE_STR_ = 537,
     _XSLT_SCHEMA_LOCATION_ = 538,
     _XSLT_TUNNEL_ = 539,
     _XSLT_REQUIRED_ = 540,
     _XSLT_OVERRIDE_ = 541,
     _XSLT_COPY_NAMESPACES_ = 542,
     _XSLT_INHERIT_NAMESPACES_ = 543,
     _XSLT_BYTE_ORDER_MARK_ = 544,
     _XSLT_ESCAPE_URI_ATTRIBUTES_ = 545,
     _XSLT_INCLUDE_CONTENT_TYPE_ = 546,
     _XSLT_INDENT_ = 547,
     _XSLT_OMIT_XML_DECLARATION_ = 548,
     _XSLT_UNDECLARE_PREFIXES_ = 549,
     _XSLT_MATCH_ = 550,
     _XSLT_AS_ = 551,
     _XSLT_SELECT_ = 552,
     _XSLT_PRIORITY_ = 553,
     _XSLT_TEST_ = 554,
     _XSLT_SEPARATOR_ = 555,
     _XSLT_NAMESPACE_A_ = 556,
     _XSLT_REGEX_ = 557,
     _XSLT_FLAGS_ = 558,
     _XSLT_METHOD_ = 559,
     _XSLT_CDATA_SECTION_ELEMENTS_ = 560,
     _XSLT_USE_CHARACTER_MAPS_ = 561,
     _XSLT_ELEMENT_NAME_ = 562,
     _XSLT_XMLNS_ATTR_ = 563,
     _XSLT_ATTR_NAME_ = 564,
     _XSLT_TEXT_NODE_ = 565,
     _XSLT_WS_TEXT_NODE_ = 566,
     _HASH_DEFAULT_ = 567,
     _HASH_ALL_ = 568,
     _HASH_CURRENT_ = 569,
     _XML_ = 570,
     _HTML_ = 571,
     _XHTML_ = 572
   };
#endif
/* Tokens.  */
#define MYEOF 0
#define _LANG_XPATH2_ 258
#define _LANG_XQUERY_ 259
#define _LANG_XQUERY_FULLTEXT_ 260
#define _LANG_XQUERY_UPDATE_ 261
#define _LANG_XQUERY_FULLTEXT_UPDATE_ 262
#define _LANG_XSLT2_ 263
#define _DOLLAR_ 264
#define _COLON_EQUALS_ 265
#define _ASTERISK_ 266
#define _BAR_ 267
#define _BANG_EQUALS_ 268
#define _LESS_THAN_EQUALS_ 269
#define _GREATER_THAN_ 270
#define _GREATER_THAN_EQUALS_ 271
#define _LESS_THAN_LESS_THAN_ 272
#define _GREATER_THAN_GREATER_THAN_ 273
#define _COLON_COLON_ 274
#define _AT_SIGN_ 275
#define _LSQUARE_ 276
#define _RSQUARE_ 277
#define _QUESTION_MARK_ 278
#define _BAR_BAR_ 279
#define _AMP_AMP_ 280
#define _LESS_THAN_OP_OR_TAG_ 281
#define _START_TAG_CLOSE_ 282
#define _END_TAG_CLOSE_ 283
#define _PI_START_ 284
#define _XML_COMMENT_START_ 285
#define _XML_COMMENT_END_ 286
#define _SLASH_SLASH_ 287
#define _END_TAG_OPEN_ 288
#define _PRAGMA_OPEN_ 289
#define _COMMA_ 290
#define _PLUS_ 291
#define _MINUS_ 292
#define _SLASH_ 293
#define _EQUALS_ 294
#define _DOT_ 295
#define _DOT_DOT_ 296
#define _LPAR_ 297
#define _RPAR_ 298
#define _EMPTY_TAG_CLOSE_ 299
#define _VALUE_INDICATOR_ 300
#define _OPEN_QUOT_ 301
#define _CLOSE_QUOT_ 302
#define _OPEN_APOS_ 303
#define _CLOSE_APOS_ 304
#define _LBRACE_ 305
#define _LBRACE_EXPR_ENCLOSURE_ 306
#define _RBRACE_ 307
#define _SEMICOLON_ 308
#define _BANG_ 309
#define _HASH_ 310
#define _INTEGER_LITERAL_ 311
#define _DECIMAL_LITERAL_ 312
#define _DOUBLE_LITERAL_ 313
#define _ATTRIBUTE_ 314
#define _COMMENT_ 315
#define _DOCUMENT_NODE_ 316
#define _NODE_ 317
#define _PROCESSING_INSTRUCTION_ 318
#define _SCHEMA_ATTRIBUTE_ 319
#define _SCHEMA_ELEMENT_ 320
#define _TEXT_ 321
#define _EMPTY_SEQUENCE_ 322
#define _BOUNDARY_SPACE_ 323
#define _FT_OPTION_ 324
#define _BASE_URI_ 325
#define _LAX_ 326
#define _STRICT_ 327
#define _IDIV_ 328
#define _CHILD_ 329
#define _DESCENDANT_ 330
#define _SELF_ 331
#define _DESCENDANT_OR_SELF_ 332
#define _FOLLOWING_SIBLING_ 333
#define _FOLLOWING_ 334
#define _PARENT_ 335
#define _ANCESTOR_ 336
#define _PRECEDING_SIBLING_ 337
#define _PRECEDING_ 338
#define _ANCESTOR_OR_SELF_ 339
#define _DOCUMENT_ 340
#define _NOT_ 341
#define _SENSITIVE_ 342
#define _INSENSITIVE_ 343
#define _DIACRITICS_ 344
#define _WITHOUT_ 345
#define _WITHOUT_C_ 346
#define _STEMMING_ 347
#define _THESAURUS_ 348
#define _STOP_ 349
#define _WILDCARDS_ 350
#define _ENTIRE_ 351
#define _CONTENT_ 352
#define _WORD_ 353
#define _TYPE_ 354
#define _START_ 355
#define _END_ 356
#define _MOST_ 357
#define _SKIP_ 358
#define _COPY_ 359
#define _VALUE_ 360
#define _WHITESPACE_ 361
#define _PI_CONTENT_ 362
#define _XML_COMMENT_CONTENT_ 363
#define _EQ_ 364
#define _NE_ 365
#define _LT_ 366
#define _LE_ 367
#define _GT_ 368
#define _GE_ 369
#define _AT_ 370
#define _QUOT_ATTR_CONTENT_ 371
#define _APOS_ATTR_CONTENT_ 372
#define _WHITESPACE_ELEMENT_CONTENT_ 373
#define _ELEMENT_CONTENT_ 374
#define _AT_LM_ 375
#define _ORDERED_ 376
#define _UNORDERED_ 377
#define _QNAME_ 378
#define _XMLNS_QNAME_ 379
#define _CONSTR_QNAME_ 380
#define _STRING_LITERAL_ 381
#define _VARIABLE_ 382
#define _NCNAME_COLON_STAR_ 383
#define _STAR_COLON_NCNAME_ 384
#define _PI_TARGET_ 385
#define _PRAGMA_CONTENT_ 386
#define _RETURN_ 387
#define _FOR_ 388
#define _IN_ 389
#define _LET_ 390
#define _WHERE_ 391
#define _BY_ 392
#define _ORDER_ 393
#define _STABLE_ 394
#define _ASCENDING_ 395
#define _DESCENDING_ 396
#define _EMPTY_ 397
#define _GREATEST_ 398
#define _LEAST_ 399
#define _COLLATION_ 400
#define _SOME_ 401
#define _EVERY_ 402
#define _SATISFIES_ 403
#define _TYPESWITCH_ 404
#define _CASE_ 405
#define _CASE_S_ 406
#define _AS_ 407
#define _IF_ 408
#define _THEN_ 409
#define _ELSE_ 410
#define _OR_ 411
#define _AND_ 412
#define _INSTANCE_ 413
#define _OF_ 414
#define _CASTABLE_ 415
#define _TO_ 416
#define _DIV_ 417
#define _MOD_ 418
#define _UNION_ 419
#define _INTERSECT_ 420
#define _EXCEPT_ 421
#define _VALIDATE_ 422
#define _CAST_ 423
#define _TREAT_ 424
#define _IS_ 425
#define _PRESERVE_ 426
#define _STRIP_ 427
#define _NAMESPACE_ 428
#define _ITEM_ 429
#define _EXTERNAL_ 430
#define _ENCODING_ 431
#define _NO_PRESERVE_ 432
#define _INHERIT_ 433
#define _NO_INHERIT_ 434
#define _DECLARE_ 435
#define _CONSTRUCTION_ 436
#define _ORDERING_ 437
#define _DEFAULT_ 438
#define _COPY_NAMESPACES_ 439
#define _OPTION_ 440
#define _XQUERY_ 441
#define _VERSION_ 442
#define _IMPORT_ 443
#define _SCHEMA_ 444
#define _MODULE_ 445
#define _ELEMENT_ 446
#define _FUNCTION_ 447
#define _FUNCTION_EXT_ 448
#define _SCORE_ 449
#define _FTCONTAINS_ 450
#define _WEIGHT_ 451
#define _WINDOW_ 452
#define _DISTANCE_ 453
#define _OCCURS_ 454
#define _TIMES_ 455
#define _SAME_ 456
#define _DIFFERENT_ 457
#define _LOWERCASE_ 458
#define _UPPERCASE_ 459
#define _RELATIONSHIP_ 460
#define _LEVELS_ 461
#define _LANGUAGE_ 462
#define _ANY_ 463
#define _ALL_ 464
#define _PHRASE_ 465
#define _EXACTLY_ 466
#define _FROM_ 467
#define _WORDS_ 468
#define _SENTENCES_ 469
#define _PARAGRAPHS_ 470
#define _SENTENCE_ 471
#define _PARAGRAPH_ 472
#define _REPLACE_ 473
#define _MODIFY_ 474
#define _FIRST_ 475
#define _INSERT_ 476
#define _BEFORE_ 477
#define _AFTER_ 478
#define _REVALIDATION_ 479
#define _WITH_ 480
#define _WITH_FT_ 481
#define _NODES_ 482
#define _RENAME_ 483
#define _LAST_ 484
#define _DELETE_ 485
#define _INTO_ 486
#define _UPDATING_ 487
#define _ID_ 488
#define _KEY_ 489
#define _TEMPLATE_ 490
#define _MATCHES_ 491
#define _NAME_ 492
#define _CALL_ 493
#define _APPLY_ 494
#define _TEMPLATES_ 495
#define _MODE_ 496
#define _XSLT_END_ELEMENT_ 497
#define _XSLT_STYLESHEET_ 498
#define _XSLT_TEMPLATE_ 499
#define _XSLT_VALUE_OF_ 500
#define _XSLT_TEXT_ 501
#define _XSLT_APPLY_TEMPLATES_ 502
#define _XSLT_CALL_TEMPLATE_ 503
#define _XSLT_WITH_PARAM_ 504
#define _XSLT_SEQUENCE_ 505
#define _XSLT_PARAM_ 506
#define _XSLT_FUNCTION_ 507
#define _XSLT_CHOOSE_ 508
#define _XSLT_WHEN_ 509
#define _XSLT_OTHERWISE_ 510
#define _XSLT_IF_ 511
#define _XSLT_VARIABLE_ 512
#define _XSLT_COMMENT_ 513
#define _XSLT_PI_ 514
#define _XSLT_DOCUMENT_ 515
#define _XSLT_ATTRIBUTE_ 516
#define _XSLT_NAMESPACE_ 517
#define _XSLT_ELEMENT_ 518
#define _XSLT_ANALYZE_STRING_ 519
#define _XSLT_MATCHING_SUBSTRING_ 520
#define _XSLT_NON_MATCHING_SUBSTRING_ 521
#define _XSLT_COPY_OF_ 522
#define _XSLT_COPY_ 523
#define _XSLT_FOR_EACH_ 524
#define _XSLT_OUTPUT_ 525
#define _XSLT_IMPORT_SCHEMA_ 526
#define _XSLT_VERSION_ 527
#define _XSLT_MODE_ 528
#define _XSLT_NAME_ 529
#define _XSLT_DOCTYPE_PUBLIC_ 530
#define _XSLT_DOCTYPE_SYSTEM_ 531
#define _XSLT_ENCODING_ 532
#define _XSLT_MEDIA_TYPE_ 533
#define _XSLT_NORMALIZATION_FORM_ 534
#define _XSLT_STANDALONE_ 535
#define _XSLT_EXCLUDE_RESULT_PREFIXES_ 536
#define _XSLT_NAMESPACE_STR_ 537
#define _XSLT_SCHEMA_LOCATION_ 538
#define _XSLT_TUNNEL_ 539
#define _XSLT_REQUIRED_ 540
#define _XSLT_OVERRIDE_ 541
#define _XSLT_COPY_NAMESPACES_ 542
#define _XSLT_INHERIT_NAMESPACES_ 543
#define _XSLT_BYTE_ORDER_MARK_ 544
#define _XSLT_ESCAPE_URI_ATTRIBUTES_ 545
#define _XSLT_INCLUDE_CONTENT_TYPE_ 546
#define _XSLT_INDENT_ 547
#define _XSLT_OMIT_XML_DECLARATION_ 548
#define _XSLT_UNDECLARE_PREFIXES_ 549
#define _XSLT_MATCH_ 550
#define _XSLT_AS_ 551
#define _XSLT_SELECT_ 552
#define _XSLT_PRIORITY_ 553
#define _XSLT_TEST_ 554
#define _XSLT_SEPARATOR_ 555
#define _XSLT_NAMESPACE_A_ 556
#define _XSLT_REGEX_ 557
#define _XSLT_FLAGS_ 558
#define _XSLT_METHOD_ 559
#define _XSLT_CDATA_SECTION_ELEMENTS_ 560
#define _XSLT_USE_CHARACTER_MAPS_ 561
#define _XSLT_ELEMENT_NAME_ 562
#define _XSLT_XMLNS_ATTR_ 563
#define _XSLT_ATTR_NAME_ 564
#define _XSLT_TEXT_NODE_ 565
#define _XSLT_WS_TEXT_NODE_ 566
#define _HASH_DEFAULT_ 567
#define _HASH_ALL_ 568
#define _HASH_CURRENT_ 569
#define _XML_ 570
#define _HTML_ 571
#define _XHTML_ 572




/* Copy the first part of user declarations.  */
#line 22 "../src/parser/XQParser.y"


#if defined(WIN32) && !defined(__CYGWIN__)
// turn off the warnings "switch statement contains 'default' but no 'case' labels"
//                       "'yyerrlab1' : unreferenced label"
#pragma warning(disable : 4065 4102)
#endif

#include <cstdio>
#include <iostream>
#include <sstream>

#include <xqilla/utils/UTF8Str.hpp>

#include "../lexer/XQLexer.hpp"

#include <xqilla/simple-api/XQQuery.hpp>
#include <xqilla/ast/XQFunction.hpp>
#include <xqilla/ast/XQDocumentConstructor.hpp>
#include <xqilla/ast/XQElementConstructor.hpp>
#include <xqilla/ast/XQAttributeConstructor.hpp>
#include <xqilla/ast/XQNamespaceConstructor.hpp>
#include <xqilla/ast/XQPIConstructor.hpp>
#include <xqilla/ast/XQCommentConstructor.hpp>
#include <xqilla/ast/XQTextConstructor.hpp>
#include <xqilla/ast/XQQuantified.hpp>
#include <xqilla/ast/XQTypeswitch.hpp>
#include <xqilla/ast/XQValidate.hpp>
#include <xqilla/ast/XQGlobalVariable.hpp>
#include <xqilla/ast/XQFunctionCall.hpp>
#include <xqilla/ast/XQOrderingChange.hpp>
#include <xqilla/ast/XQDocumentOrder.hpp>
#include <xqilla/ast/XQReturn.hpp>
#include <xqilla/ast/XQNamespaceBinding.hpp>

#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/dom/DOMXPathNSResolver.hpp>

#include <xqilla/ast/XQSequence.hpp>
#include <xqilla/ast/XQNav.hpp>
#include <xqilla/ast/XQStep.hpp>
#include <xqilla/ast/XQLiteral.hpp>
#include <xqilla/ast/XQVariable.hpp>
#include <xqilla/ast/XQInstanceOf.hpp>
#include <xqilla/ast/XQCastAs.hpp>
#include <xqilla/ast/XQCastableAs.hpp>
#include <xqilla/ast/XQTreatAs.hpp>
#include <xqilla/ast/XQFunctionConversion.hpp>
#include <xqilla/ast/XQAnalyzeString.hpp>
#include <xqilla/ast/XQCopyOf.hpp>
#include <xqilla/ast/XQCopy.hpp>
#include <xqilla/ast/XQAtomize.hpp>
#include <xqilla/ast/ConvertFunctionArg.hpp>
#include <xqilla/ast/XQIf.hpp>
#include <xqilla/ast/XQContextItem.hpp>
#include <xqilla/ast/XQPredicate.hpp>
#include <xqilla/ast/XQApplyTemplates.hpp>
#include <xqilla/ast/XQInlineFunction.hpp>
#include <xqilla/ast/XQFunctionDeref.hpp>
#include <xqilla/ast/XQFunctionRef.hpp>
#include <xqilla/ast/XQMap.hpp>

#include <xqilla/fulltext/FTContains.hpp>
#include <xqilla/fulltext/FTOr.hpp>
#include <xqilla/fulltext/FTAnd.hpp>
#include <xqilla/fulltext/FTMildnot.hpp>
#include <xqilla/fulltext/FTUnaryNot.hpp>
#include <xqilla/fulltext/FTOrder.hpp>
#include <xqilla/fulltext/FTDistance.hpp>
#include <xqilla/fulltext/FTScope.hpp>
#include <xqilla/fulltext/FTContent.hpp>
#include <xqilla/fulltext/FTWindow.hpp>

#include <xqilla/update/UDelete.hpp>
#include <xqilla/update/URename.hpp>
#include <xqilla/update/UReplace.hpp>
#include <xqilla/update/UReplaceValueOf.hpp>
#include <xqilla/update/UInsertAsFirst.hpp>
#include <xqilla/update/UInsertAsLast.hpp>
#include <xqilla/update/UInsertInto.hpp>
#include <xqilla/update/UInsertAfter.hpp>
#include <xqilla/update/UInsertBefore.hpp>
#include <xqilla/update/UTransform.hpp>
#include <xqilla/update/UApplyUpdates.hpp>

#include <xqilla/ast/ContextTuple.hpp>
#include <xqilla/ast/ForTuple.hpp>
#include <xqilla/ast/LetTuple.hpp>
#include <xqilla/ast/WhereTuple.hpp>
#include <xqilla/ast/OrderByTuple.hpp>

#include <xqilla/parser/QName.hpp>

#include <xqilla/operators/Equals.hpp>
#include <xqilla/operators/GeneralComp.hpp>
#include <xqilla/operators/NotEquals.hpp>
#include <xqilla/operators/LessThan.hpp>
#include <xqilla/operators/GreaterThan.hpp>
#include <xqilla/operators/LessThanEqual.hpp>
#include <xqilla/operators/GreaterThanEqual.hpp>
#include <xqilla/operators/Plus.hpp>
#include <xqilla/operators/Minus.hpp>
#include <xqilla/operators/Multiply.hpp>
#include <xqilla/operators/Divide.hpp>
#include <xqilla/operators/IntegerDivide.hpp>
#include <xqilla/operators/Mod.hpp>
#include <xqilla/operators/UnaryMinus.hpp>
#include <xqilla/operators/NodeComparison.hpp>
#include <xqilla/operators/OrderComparison.hpp>
#include <xqilla/operators/Intersect.hpp>
#include <xqilla/operators/Except.hpp>
#include <xqilla/operators/Union.hpp>
#include <xqilla/operators/Or.hpp>
#include <xqilla/operators/And.hpp>
#include <xqilla/operators/Range.hpp>

#include <xqilla/functions/FunctionConstructor.hpp>
#include <xqilla/functions/FunctionRoot.hpp>
#include <xqilla/functions/FunctionQName.hpp>
#include <xqilla/functions/FunctionId.hpp>
#include <xqilla/functions/FunctionExists.hpp>
#include <xqilla/functions/XQillaFunction.hpp>

#include <xqilla/axis/NodeTest.hpp>

#include <xqilla/schema/SequenceType.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/exceptions/NamespaceLookupException.hpp>
#include <xqilla/exceptions/ContextException.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/utils/XPath2NSUtils.hpp>
#include <xqilla/utils/UTF8Str.hpp>
#include "../config/xqilla_config.h"

#define YYPARSE_PARAM qp
#define YYDEBUG 1
#define YYERROR_VERBOSE

// this removes a memory leak occurring when an error is found in the query (we throw an exception from inside
// yyerror, preventing the bison-generated code from cleaning up the memory); by defining this macro we use 
// stack-based memory instead of the heap
#define YYSTACK_USE_ALLOCA  1
#if HAVE_ALLOCA_H
#include <alloca.h>
#elif defined __GNUC__
#undef alloca
#define alloca __builtin_alloca
#elif defined _AIX
#define alloca __alloca
#elif defined _MSC_VER
#include <malloc.h>
#else
#include <stddef.h>
#ifdef __cplusplus
extern "C"
#endif
void *alloca (size_t);
#endif

#define QP       ((XQParserArgs*)qp)
#define CONTEXT  (QP->_context)
#define LANGUAGE (QP->_lexer->getLanguage())
#define MEMMGR   (CONTEXT->getMemoryManager())

#define REJECT_NOT_XQUERY(where,pos)      if(!QP->_lexer->isXQuery()) { yyerror(LANGUAGE, #where, (pos).first_line, (pos).first_column); }
#define REJECT_NOT_EXTENSION(where,pos)   if(!QP->_lexer->isExtensions()) { yyerror(LANGUAGE, #where, (pos).first_line, (pos).first_column); }

#define WRAP(pos,object)        (wrapForDebug((QP), (object), (pos).first_line, (pos).first_column))

#define LOCATION(pos,name) LocationInfo name(QP->_query->getFile(), (pos).first_line, (pos).first_column)

#define GET_NAVIGATION(pos, object) (((object)->getType() == ASTNode::NAVIGATION) ? (XQNav*)(object) : WRAP(pos, new (MEMMGR) XQNav((object), MEMMGR)))

#define PRESERVE_NS(pos, object) (QP->_lexer->getNSResolver() == 0 ? (object) : WRAP((pos), new (MEMMGR) XQNamespaceBinding(QP->_lexer->getNSResolver(), (object), MEMMGR)))

#define BIT_ORDERING_SPECIFIED                  0
#define BIT_BOUNDARY_SPECIFIED                  1
#define BIT_COLLATION_SPECIFIED                 2
#define BIT_BASEURI_SPECIFIED                   3
#define BIT_CONSTRUCTION_SPECIFIED              4
#define BIT_EMPTYORDERING_SPECIFIED             5
#define BIT_COPYNAMESPACE_SPECIFIED             6
#define BIT_DEFAULTELEMENTNAMESPACE_SPECIFIED   7
#define BIT_DEFAULTFUNCTIONNAMESPACE_SPECIFIED  8
#define BIT_DECLARE_SECOND_STEP                 9
#define BIT_REVALIDATION_SPECIFIED              10

#define CHECK_SECOND_STEP(pos, name) { \
  if(QP->_flags.get(BIT_DECLARE_SECOND_STEP)) \
    yyerror((pos), "Prolog contains " name " declaration after a variable, function or option declaration"); \
}

#define CHECK_SPECIFIED(pos, bit, name, error) { \
  if(QP->_flags.get(bit)) \
    yyerror((pos), "Prolog contains more than one " name " declaration [err:" error "]"); \
  QP->_flags.set(bit); \
}

#define SET_NAMESPACE(pos, prefix, uri) { \
  if(QP->_namespaceDecls.containsKey((prefix))) \
    yyerror((pos), "Namespace prefix has already been bound to a namespace [err:XQST0033]"); \
  QP->_namespaceDecls.put((prefix), NULL); \
  CONTEXT->setNamespaceBinding((prefix), (uri)); \
}

#undef yylex
#define yylex QP->_lexer->yylex
#undef yyerror
#define yyerror QP->_lexer->error

XERCES_CPP_NAMESPACE_USE;
using namespace std;

static const XMLCh sz1_0[] = { chDigit_1, chPeriod, chDigit_0, chNull };
static const XMLCh option_projection[] = { 'p', 'r', 'o', 'j', 'e', 'c', 't', 'i', 'o', 'n', 0 };
static const XMLCh option_psvi[] = { 'p', 's', 'v', 'i', 0 };
static const XMLCh option_lint[] = { 'l', 'i', 'n', 't', 0 };
static const XMLCh var_name[] = { 'n', 'a', 'm', 'e', 0 };

static inline VectorOfASTNodes packageArgs(ASTNode *arg1Impl, ASTNode *arg2Impl, XPath2MemoryManager* memMgr)
{
  VectorOfASTNodes args=VectorOfASTNodes(2,(ASTNode*)NULL,XQillaAllocator<ASTNode*>(memMgr));
  args[0]=arg1Impl;
  args[1]=arg2Impl;

  return args;
}

static inline TupleNode *getLastAncestor(TupleNode *node)
{
  while(node->getParent()) node = const_cast<TupleNode*>(node->getParent());
  return node;
}

static inline TupleNode *setLastAncestor(TupleNode *node, TupleNode *ancestor)
{
  getLastAncestor(node)->setParent(ancestor);
  return node;
}

template<typename TYPE>
TYPE *wrapForDebug(XQParserArgs *qp, TYPE* pObjToWrap, unsigned int line, unsigned int column)
{
  if(pObjToWrap->getLine() == 0)
    pObjToWrap->setLocationInfo(QP->_query->getFile(), line, column);
  return pObjToWrap;
}

#define RESOLVE_QNAME(pos, qname) const XMLCh *uri, *name; resolveQName((pos), QP, (qname), uri, name)

static void resolveQName(const yyltype &pos, XQParserArgs *qp, const XMLCh *qname, const XMLCh *&uri, const XMLCh *&name)
{
  const XMLCh *prefix = XPath2NSUtils::getPrefix(qname, MEMMGR);
  name = XPath2NSUtils::getLocalName(qname);

  if(prefix == 0 || *prefix == 0) {
    uri = 0;
  }
  else {
    if(QP->_lexer->getNSResolver() == 0) {
      if(prefix == 0 || *prefix == 0)
        uri = CONTEXT->getDefaultElementAndTypeNS();
      else uri = CONTEXT->getNSResolver()->lookupNamespaceURI(prefix);
    }
    else {
      uri = QP->_lexer->getNSResolver()->lookupNamespaceURI(prefix);
    }

    if(uri == 0 || *uri == 0) {
      ostringstream oss;
      oss << "No namespace binding for prefix '" << UTF8(prefix) << "' [err:XTSE0280]";
      yyerror(pos, oss.str().c_str());
    }
  }
}

#define XSLT_VARIABLE_VALUE(pos, select, seqConstruct, seqType) variableValueXSLT((pos), QP, (select), (seqConstruct), (seqType))

static ASTNode *variableValueXSLT(const yyltype &pos, XQParserArgs *qp, ASTNode *select, XQSequence *seqConstruct, SequenceType *seqType)
{
  if(!seqConstruct->getChildren().empty()) {
    if(select != 0) return 0; // signifies an error
    
    if(seqType == 0) {
      return WRAP(pos, new (MEMMGR) XQDocumentConstructor(seqConstruct, MEMMGR));
    }

    return WRAP(pos, new (MEMMGR) XQFunctionConversion(seqConstruct, seqType, MEMMGR));
  }

  if(select != 0) {
    if(seqType == 0) return select;

    return WRAP(pos, new (MEMMGR) XQFunctionConversion(select, seqType, MEMMGR));
  }

  if(seqType == 0) {
    return WRAP(pos, new (MEMMGR) XQLiteral(
                  SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                  SchemaSymbols::fgDT_STRING,
                  XMLUni::fgZeroLenString, AnyAtomicType::STRING,
                  MEMMGR));
  }

  return WRAP(pos, new (MEMMGR) XQFunctionConversion(WRAP(pos, new (MEMMGR) XQSequence(MEMMGR)), seqType, MEMMGR));
}

namespace XQParser {



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 1065 "../src/parser/XQParser.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  318
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   8260

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  318
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  329
/* YYNRULES -- Number of rules.  */
#define YYNRULES  996
/* YYNRULES -- Number of states.  */
#define YYNSTATES  1640

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   572

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    15,    18,    21,    23,
      28,    30,    31,    34,    37,    38,    41,    44,    47,    50,
      53,    56,    61,    63,    67,    70,    74,    78,    82,    83,
      86,    89,    92,    94,    96,    98,   103,   105,   108,   112,
     115,   116,   119,   123,   125,   128,   132,   136,   139,   142,
     146,   148,   151,   155,   159,   162,   165,   169,   171,   174,
     178,   182,   185,   187,   190,   194,   197,   201,   204,   207,
     210,   213,   216,   219,   222,   225,   228,   231,   234,   238,
     241,   243,   245,   247,   249,   251,   253,   256,   259,   261,
     264,   267,   271,   275,   280,   281,   285,   288,   289,   292,
     295,   296,   299,   302,   305,   307,   309,   311,   313,   315,
     317,   319,   321,   323,   325,   327,   329,   331,   333,   335,
     337,   339,   341,   345,   347,   351,   355,   359,   361,   363,
     367,   369,   373,   377,   379,   381,   383,   384,   387,   391,
     393,   396,   397,   400,   404,   406,   409,   413,   417,   420,
     423,   427,   431,   435,   440,   442,   445,   449,   453,   454,
     458,   463,   465,   469,   473,   477,   478,   482,   483,   487,
     492,   494,   497,   501,   505,   509,   511,   515,   519,   521,
     525,   529,   533,   535,   539,   541,   545,   549,   553,   557,
     561,   563,   567,   571,   575,   577,   581,   585,   588,   590,
     594,   597,   601,   603,   606,   609,   613,   615,   619,   623,
     625,   627,   629,   631,   633,   635,   640,   647,   649,   651,
     653,   655,   657,   660,   664,   668,   672,   676,   679,   681,
     685,   689,   692,   693,   698,   701,   704,   706,   708,   711,
     714,   715,   719,   723,   727,   731,   735,   739,   743,   747,
     749,   751,   753,   755,   757,   759,   761,   767,   775,   781,
     789,   791,   793,   796,   799,   801,   803,   806,   809,   810,
     814,   818,   822,   826,   830,   834,   838,   842,   846,   849,
     852,   854,   856,   859,   862,   863,   867,   871,   875,   879,
     883,   887,   891,   895,   898,   901,   903,   905,   910,   917,
     920,   923,   930,   931,   935,   939,   943,   947,   951,   955,
     959,   963,   967,   969,   971,   973,   975,   977,   979,   981,
     983,   985,   987,   989,   995,   999,  1003,  1009,  1015,  1020,
    1024,  1028,  1032,  1038,  1044,  1050,  1052,  1054,  1056,  1058,
    1063,  1067,  1073,  1078,  1079,  1082,  1086,  1090,  1094,  1102,
    1107,  1114,  1117,  1119,  1123,  1127,  1134,  1143,  1150,  1159,
    1162,  1166,  1167,  1169,  1173,  1175,  1179,  1183,  1187,  1189,
    1193,  1195,  1197,  1199,  1201,  1203,  1205,  1207,  1209,  1211,
    1213,  1215,  1217,  1219,  1223,  1227,  1230,  1233,  1235,  1238,
    1241,  1243,  1245,  1248,  1252,  1254,  1262,  1263,  1267,  1268,
    1272,  1275,  1279,  1281,  1288,  1294,  1297,  1301,  1306,  1310,
    1312,  1316,  1322,  1324,  1325,  1327,  1329,  1330,  1333,  1336,
    1341,  1346,  1350,  1352,  1358,  1365,  1368,  1370,  1376,  1380,
    1385,  1393,  1402,  1406,  1408,  1412,  1414,  1418,  1422,  1423,
    1428,  1432,  1436,  1440,  1444,  1448,  1452,  1456,  1460,  1464,
    1468,  1472,  1476,  1478,  1482,  1487,  1489,  1493,  1495,  1499,
    1503,  1505,  1509,  1513,  1517,  1521,  1523,  1527,  1531,  1533,
    1537,  1541,  1543,  1548,  1550,  1555,  1557,  1562,  1564,  1569,
    1571,  1574,  1577,  1579,  1581,  1583,  1585,  1590,  1596,  1602,
    1606,  1611,  1613,  1616,  1621,  1623,  1626,  1628,  1631,  1634,
    1636,  1638,  1642,  1646,  1648,  1650,  1652,  1655,  1658,  1661,
    1663,  1666,  1669,  1672,  1675,  1678,  1681,  1684,  1687,  1690,
    1692,  1695,  1697,  1700,  1703,  1706,  1709,  1712,  1714,  1716,
    1718,  1720,  1722,  1724,  1726,  1728,  1731,  1732,  1737,  1739,
    1741,  1743,  1745,  1747,  1749,  1751,  1753,  1755,  1757,  1759,
    1761,  1763,  1765,  1767,  1770,  1772,  1776,  1779,  1781,  1786,
    1791,  1795,  1800,  1804,  1806,  1808,  1810,  1812,  1814,  1816,
    1822,  1833,  1835,  1837,  1838,  1846,  1854,  1855,  1857,  1861,
    1865,  1869,  1873,  1874,  1877,  1880,  1881,  1884,  1887,  1888,
    1891,  1894,  1895,  1898,  1901,  1902,  1905,  1908,  1911,  1914,
    1918,  1919,  1921,  1925,  1927,  1930,  1932,  1934,  1936,  1938,
    1940,  1942,  1947,  1951,  1953,  1957,  1960,  1964,  1968,  1973,
    1978,  1982,  1984,  1988,  1991,  1995,  1998,  1999,  2001,  2002,
    2005,  2008,  2012,  2013,  2015,  2017,  2019,  2021,  2025,  2027,
    2029,  2031,  2033,  2035,  2037,  2039,  2041,  2043,  2045,  2047,
    2049,  2051,  2055,  2059,  2064,  2069,  2073,  2077,  2081,  2086,
    2091,  2095,  2100,  2107,  2109,  2111,  2116,  2118,  2122,  2127,
    2134,  2142,  2144,  2146,  2151,  2153,  2155,  2157,  2159,  2161,
    2166,  2169,  2170,  2173,  2176,  2180,  2182,  2186,  2188,  2193,
    2195,  2198,  2200,  2204,  2206,  2209,  2212,  2215,  2218,  2223,
    2225,  2229,  2233,  2237,  2240,  2243,  2246,  2249,  2252,  2254,
    2256,  2258,  2260,  2262,  2264,  2266,  2268,  2270,  2273,  2276,
    2279,  2282,  2285,  2288,  2291,  2294,  2298,  2302,  2309,  2316,
    2319,  2320,  2324,  2327,  2332,  2337,  2344,  2350,  2354,  2360,
    2361,  2364,  2367,  2371,  2373,  2377,  2380,  2383,  2386,  2389,
    2392,  2393,  2395,  2398,  2400,  2403,  2405,  2408,  2412,  2416,
    2421,  2423,  2425,  2427,  2429,  2431,  2435,  2439,  2443,  2447,
    2454,  2461,  2466,  2471,  2476,  2479,  2482,  2485,  2488,  2491,
    2499,  2505,  2511,  2518,  2522,  2524,  2529,  2531,  2533,  2535,
    2537,  2539,  2541,  2547,  2555,  2564,  2575,  2582,  2591,  2592,
    2595,  2598,  2602,  2603,  2606,  2608,  2612,  2614,  2616,  2618,
    2622,  2630,  2634,  2642,  2648,  2658,  2660,  2662,  2664,  2666,
    2670,  2676,  2678,  2680,  2684,  2689,  2693,  2698,  2702,  2708,
    2715,  2717,  2721,  2725,  2727,  2729,  2731,  2733,  2735,  2737,
    2739,  2741,  2743,  2745,  2747,  2749,  2751,  2753,  2755,  2757,
    2759,  2761,  2763,  2765,  2767,  2769,  2771,  2773,  2775,  2777,
    2779,  2781,  2783,  2785,  2787,  2789,  2791,  2793,  2795,  2797,
    2799,  2801,  2803,  2805,  2807,  2809,  2811,  2813,  2815,  2817,
    2819,  2821,  2823,  2825,  2827,  2829,  2831,  2833,  2835,  2837,
    2839,  2841,  2843,  2845,  2847,  2849,  2851,  2853,  2855,  2857,
    2859,  2861,  2863,  2865,  2867,  2869,  2871,  2873,  2875,  2877,
    2879,  2881,  2883,  2885,  2887,  2889,  2891,  2893,  2895,  2897,
    2899,  2901,  2903,  2905,  2907,  2909,  2911,  2913,  2915,  2917,
    2919,  2921,  2923,  2925,  2927,  2929,  2931,  2933,  2935,  2937,
    2939,  2941,  2943,  2945,  2947,  2949,  2951,  2953,  2955,  2957,
    2959,  2961,  2963,  2965,  2967,  2969,  2971,  2973,  2975,  2977,
    2979,  2981,  2983,  2985,  2987,  2989,  2991,  2993,  2995,  2997,
    2999,  3001,  3003,  3005,  3007,  3009,  3011,  3013,  3015,  3017,
    3019,  3021,  3023,  3025,  3027,  3029,  3031,  3033,  3035,  3037,
    3039,  3041,  3043,  3045,  3047,  3049,  3051
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     319,     0,    -1,     3,   453,    -1,     4,   404,    -1,     5,
     411,    -1,     6,   415,    -1,     7,   419,    -1,     8,   320,
      -1,   321,    -1,   243,   322,   323,   242,    -1,   343,    -1,
      -1,   322,   272,    -1,   322,   281,    -1,    -1,   323,   324,
      -1,   323,   328,    -1,   323,   333,    -1,   323,   335,    -1,
     323,   337,    -1,   323,   341,    -1,   325,   330,   346,   242,
      -1,   244,    -1,   325,   295,   394,    -1,   325,   274,    -1,
     325,   298,   327,    -1,   325,   273,   326,    -1,   325,   296,
     561,    -1,    -1,   326,   123,    -1,   326,   312,    -1,   326,
     313,    -1,    56,    -1,    57,    -1,    58,    -1,   329,   330,
     346,   242,    -1,   252,    -1,   329,   274,    -1,   329,   296,
     561,    -1,   329,   286,    -1,    -1,   330,   331,    -1,   332,
     346,   242,    -1,   251,    -1,   332,   274,    -1,   332,   297,
     454,    -1,   332,   296,   561,    -1,   332,   285,    -1,   332,
     284,    -1,   334,   346,   242,    -1,   251,    -1,   334,   274,
      -1,   334,   297,   454,    -1,   334,   296,   561,    -1,   334,
     285,    -1,   334,   284,    -1,   336,   346,   242,    -1,   257,
      -1,   336,   274,    -1,   336,   297,   454,    -1,   336,   296,
     561,    -1,   338,   242,    -1,   270,    -1,   338,   274,    -1,
     338,   304,   339,    -1,   338,   289,    -1,   338,   305,   340,
      -1,   338,   275,    -1,   338,   276,    -1,   338,   277,    -1,
     338,   290,    -1,   338,   291,    -1,   338,   292,    -1,   338,
     278,    -1,   338,   279,    -1,   338,   293,    -1,   338,   280,
      -1,   338,   294,    -1,   338,   306,   340,    -1,   338,   272,
      -1,   315,    -1,   316,    -1,   317,    -1,    66,    -1,   123,
      -1,   123,    -1,   340,   123,    -1,   342,   242,    -1,   271,
      -1,   271,   282,    -1,   271,   283,    -1,   271,   282,   283,
      -1,   271,   283,   282,    -1,   307,   344,   346,   242,    -1,
      -1,   344,   309,   345,    -1,   344,   308,    -1,    -1,   345,
     452,    -1,   345,   116,    -1,    -1,   346,   351,    -1,   346,
     343,    -1,   346,   347,    -1,   348,    -1,   350,    -1,   352,
      -1,   356,    -1,   361,    -1,   365,    -1,   363,    -1,   374,
      -1,   376,    -1,   378,    -1,   380,    -1,   382,    -1,   384,
      -1,   386,    -1,   370,    -1,   388,    -1,   390,    -1,   392,
      -1,   349,   346,   242,    -1,   245,    -1,   349,   297,   454,
      -1,   349,   300,   345,    -1,   246,   351,   242,    -1,   310,
      -1,   311,    -1,   353,   355,   242,    -1,   247,    -1,   353,
     297,   454,    -1,   353,   273,   354,    -1,   123,    -1,   312,
      -1,   314,    -1,    -1,   355,   359,    -1,   357,   358,   242,
      -1,   248,    -1,   357,   274,    -1,    -1,   358,   359,    -1,
     360,   346,   242,    -1,   249,    -1,   360,   274,    -1,   360,
     297,   454,    -1,   360,   296,   561,    -1,   360,   284,    -1,
     362,   242,    -1,   250,   297,   454,    -1,   364,   346,   242,
      -1,   256,   299,   454,    -1,   253,   366,   369,   242,    -1,
     367,    -1,   366,   367,    -1,   368,   346,   242,    -1,   254,
     299,   454,    -1,    -1,   255,   346,   242,    -1,   371,   372,
     373,   242,    -1,   264,    -1,   371,   297,   454,    -1,   371,
     302,   345,    -1,   371,   303,   345,    -1,    -1,   265,   346,
     242,    -1,    -1,   266,   346,   242,    -1,   375,   346,   242,
     346,    -1,   257,    -1,   375,   274,    -1,   375,   297,   454,
      -1,   375,   296,   561,    -1,   377,   346,   242,    -1,   258,
      -1,   377,   297,   454,    -1,   379,   346,   242,    -1,   259,
      -1,   379,   274,   345,    -1,   379,   297,   454,    -1,   381,
     346,   242,    -1,   260,    -1,   383,   346,   242,    -1,   261,
      -1,   383,   274,   345,    -1,   383,   301,   345,    -1,   383,
     297,   454,    -1,   383,   300,   345,    -1,   385,   346,   242,
      -1,   262,    -1,   385,   274,   345,    -1,   385,   297,   454,
      -1,   387,   346,   242,    -1,   263,    -1,   387,   274,   345,
      -1,   387,   301,   345,    -1,   389,   242,    -1,   267,    -1,
     389,   297,   454,    -1,   389,   287,    -1,   391,   346,   242,
      -1,   268,    -1,   391,   287,    -1,   391,   288,    -1,   393,
     346,   242,    -1,   269,    -1,   393,   297,   454,    -1,   394,
      12,   395,    -1,   395,    -1,   400,    -1,    38,    -1,   397,
      -1,    38,    -1,    32,    -1,   233,    42,   398,    43,    -1,
     234,    42,   624,    35,   399,    43,    -1,   624,    -1,   522,
      -1,   520,    -1,   522,    -1,   401,    -1,   396,   401,    -1,
     397,    38,   401,    -1,   397,    32,   401,    -1,   400,    38,
     401,    -1,   400,    32,   401,    -1,   402,   403,    -1,   514,
      -1,    74,    19,   514,    -1,    59,    19,   514,    -1,    20,
     514,    -1,    -1,   518,    21,   454,    22,    -1,   420,   405,
      -1,   420,   406,    -1,   405,    -1,   406,    -1,   407,   453,
      -1,   423,   407,    -1,    -1,   407,   408,   427,    -1,   407,
     426,   427,    -1,   407,   428,   427,    -1,   407,   430,   427,
      -1,   407,   444,   427,    -1,   407,   409,   427,    -1,   407,
     627,   427,    -1,   407,   431,   427,    -1,   429,    -1,   438,
      -1,   439,    -1,   446,    -1,   433,    -1,   434,    -1,   435,
      -1,   180,   410,   646,   448,   452,    -1,   180,   410,   646,
     448,   152,   561,   452,    -1,   180,   410,   646,   448,   175,
      -1,   180,   410,   646,   448,   152,   561,   175,    -1,   192,
      -1,   193,    -1,   420,   412,    -1,   420,   413,    -1,   412,
      -1,   413,    -1,   414,   453,    -1,   423,   414,    -1,    -1,
     414,   408,   427,    -1,   414,   426,   427,    -1,   414,   428,
     427,    -1,   414,   430,   427,    -1,   414,   444,   427,    -1,
     414,   409,   427,    -1,   414,   627,   427,    -1,   414,   431,
     427,    -1,   414,   432,   427,    -1,   420,   416,    -1,   420,
     417,    -1,   416,    -1,   417,    -1,   418,   453,    -1,   423,
     418,    -1,    -1,   418,   425,   427,    -1,   418,   426,   427,
      -1,   418,   428,   427,    -1,   418,   430,   427,    -1,   418,
     444,   427,    -1,   418,   447,   427,    -1,   418,   627,   427,
      -1,   418,   431,   427,    -1,   420,   421,    -1,   420,   422,
      -1,   421,    -1,   422,    -1,   186,   187,   126,   427,    -1,
     186,   187,   126,   176,   126,   427,    -1,   424,   453,    -1,
     423,   424,    -1,   190,   173,   625,    39,   582,   427,    -1,
      -1,   424,   425,   427,    -1,   424,   426,   427,    -1,   424,
     428,   427,    -1,   424,   430,   427,    -1,   424,   444,   427,
      -1,   424,   447,   427,    -1,   424,   627,   427,    -1,   424,
     431,   427,    -1,   424,   432,   427,    -1,   429,    -1,   438,
      -1,   439,    -1,   446,    -1,   433,    -1,   434,    -1,   611,
      -1,   435,    -1,   440,    -1,   443,    -1,    53,    -1,   180,
     173,   625,    39,   582,    -1,   180,    68,   171,    -1,   180,
      68,   172,    -1,   180,   183,   191,   173,   582,    -1,   180,
     183,   410,   173,   582,    -1,   180,   185,   645,   126,    -1,
     180,    69,   592,    -1,   180,   182,   121,    -1,   180,   182,
     122,    -1,   180,   183,   138,   142,   143,    -1,   180,   183,
     138,   142,   144,    -1,   180,   184,   436,    35,   437,    -1,
     171,    -1,   177,    -1,   178,    -1,   179,    -1,   180,   183,
     145,   582,    -1,   180,    70,   582,    -1,   188,   189,   442,
     582,   441,    -1,   188,   189,   582,   441,    -1,    -1,   115,
     582,    -1,   441,    35,   582,    -1,   173,   625,    39,    -1,
     183,   191,   173,    -1,   188,   190,   173,   625,    39,   582,
     441,    -1,   188,   190,   582,   441,    -1,   180,   127,     9,
     523,   560,   445,    -1,    10,   455,    -1,   175,    -1,   180,
     181,   171,    -1,   180,   181,   172,    -1,   180,   449,   410,
     646,   448,   452,    -1,   180,   449,   410,   646,   448,   152,
     561,   452,    -1,   180,   449,   410,   646,   448,   175,    -1,
     180,   449,   410,   646,   448,   152,   561,   175,    -1,    42,
      43,    -1,    42,   450,    43,    -1,    -1,   232,    -1,   450,
      35,   451,    -1,   451,    -1,     9,   523,   560,    -1,    50,
     454,    52,    -1,    51,   454,    52,    -1,   454,    -1,   454,
      35,   455,    -1,   455,    -1,   456,    -1,   474,    -1,   477,
      -1,   481,    -1,   612,    -1,   614,    -1,   617,    -1,   616,
      -1,   618,    -1,   482,    -1,   633,    -1,   634,    -1,   457,
     132,   455,    -1,   458,   467,   468,    -1,   458,   467,    -1,
     458,   468,    -1,   458,    -1,   458,   459,    -1,   458,   464,
      -1,   459,    -1,   464,    -1,   133,   460,    -1,   460,    35,
     461,    -1,   461,    -1,     9,   523,   560,   462,   463,   134,
     455,    -1,    -1,   115,     9,   523,    -1,    -1,   194,     9,
     523,    -1,   135,   465,    -1,   465,    35,   466,    -1,   466,
      -1,     9,   523,   560,   463,    10,   455,    -1,   194,     9,
     523,    10,   455,    -1,   136,   455,    -1,   138,   137,   469,
      -1,   139,   138,   137,   469,    -1,   469,    35,   470,    -1,
     470,    -1,   471,   472,   473,    -1,   471,   472,   473,   145,
     582,    -1,   455,    -1,    -1,   140,    -1,   141,    -1,    -1,
     142,   143,    -1,   142,   144,    -1,   146,   475,   148,   455,
      -1,   147,   475,   148,   455,    -1,   475,    35,   476,    -1,
     476,    -1,     9,   523,   560,   134,   455,    -1,   149,    42,
     454,    43,   478,   479,    -1,   478,   480,    -1,   480,    -1,
     183,     9,   523,   132,   455,    -1,   183,   132,   455,    -1,
     150,   561,   132,   455,    -1,   150,     9,   523,   152,   561,
     132,   455,    -1,   153,    42,   454,    43,   154,   455,   155,
     455,    -1,   482,   156,   483,    -1,   483,    -1,   483,   157,
     484,    -1,   484,    -1,   486,    39,   486,    -1,   486,    13,
     486,    -1,    -1,   486,    26,   485,   486,    -1,   486,    14,
     486,    -1,   486,    15,   486,    -1,   486,    16,   486,    -1,
     486,   109,   486,    -1,   486,   110,   486,    -1,   486,   111,
     486,    -1,   486,   112,   486,    -1,   486,   113,   486,    -1,
     486,   114,   486,    -1,   486,   170,   486,    -1,   486,    17,
     486,    -1,   486,    18,   486,    -1,   486,    -1,   487,   195,
     583,    -1,   487,   195,   583,   610,    -1,   487,    -1,   488,
     161,   488,    -1,   488,    -1,   488,    36,   489,    -1,   488,
      37,   489,    -1,   489,    -1,   489,    11,   490,    -1,   489,
     162,   490,    -1,   489,    73,   490,    -1,   489,   163,   490,
      -1,   490,    -1,   490,    12,   491,    -1,   490,   164,   491,
      -1,   491,    -1,   491,   165,   492,    -1,   491,   166,   492,
      -1,   492,    -1,   493,   158,   159,   561,    -1,   493,    -1,
     494,   169,   152,   561,    -1,   494,    -1,   495,   160,   152,
     558,    -1,   495,    -1,   496,   168,   152,   558,    -1,   496,
      -1,    37,   496,    -1,    36,   496,    -1,   497,    -1,   498,
      -1,   503,    -1,   499,    -1,   167,    51,   454,    52,    -1,
     167,    71,    51,   454,    52,    -1,   167,    72,    51,   454,
      52,    -1,   500,    51,    52,    -1,   500,    51,   454,    52,
      -1,   501,    -1,   500,   501,    -1,    34,   535,   645,   502,
      -1,   131,    -1,   106,   131,    -1,   504,    -1,   504,   505,
      -1,    32,   505,    -1,   505,    -1,    38,    -1,   505,    38,
     506,    -1,   505,    32,   506,    -1,   506,    -1,   507,    -1,
     517,    -1,   508,   518,    -1,   511,   518,    -1,   509,   514,
      -1,   510,    -1,    74,    19,    -1,    75,    19,    -1,    59,
      19,    -1,    76,    19,    -1,    77,    19,    -1,    78,    19,
      -1,    79,    19,    -1,   173,    19,    -1,    20,   514,    -1,
     514,    -1,   512,   514,    -1,   513,    -1,    80,    19,    -1,
      81,    19,    -1,    82,    19,    -1,    83,    19,    -1,    84,
      19,    -1,    41,    -1,   565,    -1,   515,    -1,   626,    -1,
     516,    -1,    11,    -1,   128,    -1,   129,    -1,   519,   518,
      -1,    -1,   518,    21,   454,    22,    -1,   520,    -1,   522,
      -1,   524,    -1,   525,    -1,   528,    -1,   530,    -1,   526,
      -1,   527,    -1,   638,    -1,   641,    -1,   521,    -1,   624,
      -1,   621,    -1,   622,    -1,   623,    -1,     9,   523,    -1,
     645,    -1,    42,   454,    43,    -1,    42,    43,    -1,    40,
      -1,   121,    51,   454,    52,    -1,   122,    51,   454,    52,
      -1,   646,    42,    43,    -1,   646,    42,   529,    43,    -1,
     529,    35,   455,    -1,   455,    -1,   531,    -1,   547,    -1,
     532,    -1,   543,    -1,   545,    -1,    26,   533,   534,   535,
      44,    -1,    26,   533,   534,   535,    27,   542,    33,   533,
     535,    28,    -1,   123,    -1,   124,    -1,    -1,   534,   106,
     123,   535,    45,   535,   536,    -1,   534,   106,   124,   535,
      45,   535,   537,    -1,    -1,   106,    -1,    46,   538,    47,
      -1,    48,   540,    49,    -1,    46,   539,    47,    -1,    48,
     541,    49,    -1,    -1,   538,   452,    -1,   538,   116,    -1,
      -1,   539,   452,    -1,   539,   116,    -1,    -1,   540,   452,
      -1,   540,   117,    -1,    -1,   541,   452,    -1,   541,   117,
      -1,    -1,   542,   531,    -1,   542,   452,    -1,   542,   119,
      -1,   542,   118,    -1,    30,   544,    31,    -1,    -1,   108,
      -1,    29,   130,   546,    -1,   107,    -1,   106,   107,    -1,
     548,    -1,   549,    -1,   552,    -1,   553,    -1,   554,    -1,
     555,    -1,    85,    51,   454,    52,    -1,   191,   550,   551,
      -1,   125,    -1,    51,   454,    52,    -1,    51,    52,    -1,
      51,   454,    52,    -1,    59,   550,   551,    -1,    66,    51,
     454,    52,    -1,    60,    51,   454,    52,    -1,    63,   556,
     557,    -1,   125,    -1,    51,   454,    52,    -1,    51,    52,
      -1,    51,   454,    52,    -1,   564,   559,    -1,    -1,    23,
      -1,    -1,   152,   561,    -1,   563,   562,    -1,    67,    42,
      43,    -1,    -1,    11,    -1,    36,    -1,    23,    -1,   564,
      -1,   174,    42,    43,    -1,   565,    -1,   642,    -1,   644,
      -1,   626,    -1,   567,    -1,   575,    -1,   571,    -1,   577,
      -1,   573,    -1,   570,    -1,   569,    -1,   568,    -1,   566,
      -1,    62,    42,    43,    -1,    61,    42,    43,    -1,    61,
      42,   575,    43,    -1,    61,    42,   577,    43,    -1,    66,
      42,    43,    -1,    60,    42,    43,    -1,    63,    42,    43,
      -1,    63,    42,   625,    43,    -1,    63,    42,   126,    43,
      -1,    59,    42,    43,    -1,    59,    42,   572,    43,    -1,
      59,    42,   572,    35,   581,    43,    -1,   579,    -1,    11,
      -1,    64,    42,   574,    43,    -1,   579,    -1,   191,    42,
      43,    -1,   191,    42,   576,    43,    -1,   191,    42,   576,
      35,   581,    43,    -1,   191,    42,   576,    35,   581,    23,
      43,    -1,   580,    -1,    11,    -1,    65,    42,   578,    43,
      -1,   580,    -1,   626,    -1,   626,    -1,   626,    -1,   126,
      -1,   585,   584,   196,   622,    -1,   585,   584,    -1,    -1,
     584,   592,    -1,   584,   591,    -1,   585,    24,   586,    -1,
     586,    -1,   586,    25,   587,    -1,   587,    -1,   587,    86,
     134,   588,    -1,   588,    -1,    54,   589,    -1,   589,    -1,
      42,   583,    43,    -1,   590,    -1,   520,   606,    -1,   522,
     606,    -1,   525,   606,    -1,   528,   606,    -1,    51,   454,
      52,   606,    -1,   121,    -1,   197,   490,   608,    -1,   198,
     607,   608,    -1,   199,   607,   200,    -1,   201,   609,    -1,
     202,   609,    -1,   115,   100,    -1,   115,   101,    -1,    96,
      97,    -1,   593,    -1,   594,    -1,   595,    -1,   596,    -1,
     599,    -1,   604,    -1,   605,    -1,   203,    -1,   204,    -1,
     151,    87,    -1,   151,    88,    -1,   226,    89,    -1,    90,
      89,    -1,    89,    87,    -1,    89,    88,    -1,   226,    92,
      -1,    90,    92,    -1,   226,    93,   598,    -1,   226,    93,
     183,    -1,   226,    93,    42,   598,   597,    43,    -1,   226,
      93,    42,   183,   597,    43,    -1,    90,    93,    -1,    -1,
     597,    35,   598,    -1,   115,   624,    -1,   115,   624,   205,
     624,    -1,   115,   624,   607,   206,    -1,   115,   624,   205,
     624,   607,   206,    -1,   226,    94,   213,   601,   600,    -1,
      90,    94,   213,    -1,   226,   183,    94,   213,   600,    -1,
      -1,   600,   603,    -1,   115,   624,    -1,    42,   602,    43,
      -1,   624,    -1,   602,    35,   624,    -1,   164,   601,    -1,
     166,   601,    -1,   207,   624,    -1,   226,    95,    -1,    90,
      95,    -1,    -1,   208,    -1,   208,    98,    -1,   209,    -1,
     209,   213,    -1,   210,    -1,   211,   490,    -1,   120,   144,
     490,    -1,   120,   102,   490,    -1,   212,   490,   161,   490,
      -1,   213,    -1,   214,    -1,   215,    -1,   216,    -1,   217,
      -1,    91,    97,   490,    -1,   180,   224,    72,    -1,   180,
     224,    71,    -1,   180,   224,   103,    -1,   613,   455,   152,
     220,   231,   455,    -1,   613,   455,   152,   229,   231,   455,
      -1,   613,   455,   231,   455,    -1,   613,   455,   223,   455,
      -1,   613,   455,   222,   455,    -1,   221,    62,    -1,   221,
     227,    -1,   615,   455,    -1,   230,    62,    -1,   230,   227,
      -1,   218,   105,   159,    62,   455,   225,   455,    -1,   218,
      62,   455,   225,   455,    -1,   228,    62,   455,   152,   455,
      -1,   104,   619,   219,   455,   132,   455,    -1,   619,    35,
     620,    -1,   620,    -1,     9,   523,    10,   455,    -1,    56,
      -1,    57,    -1,    58,    -1,   126,    -1,   645,    -1,   645,
      -1,   180,   235,   237,   645,   452,    -1,   180,   235,   237,
     645,   629,   628,   452,    -1,   180,   235,   237,   645,   630,
     236,   394,   452,    -1,   180,   235,   237,   645,   630,   236,
     394,   629,   628,   452,    -1,   180,   235,   630,   236,   394,
     452,    -1,   180,   235,   630,   236,   394,   629,   628,   452,
      -1,    -1,   152,   561,    -1,    42,    43,    -1,    42,   450,
      43,    -1,    -1,   241,   631,    -1,   632,    -1,   631,    35,
     632,    -1,   123,    -1,   312,    -1,   313,    -1,   238,   235,
     645,    -1,   238,   235,   645,   225,    42,   636,    43,    -1,
     239,   240,   455,    -1,   239,   240,   455,   225,    42,   636,
      43,    -1,   239,   240,   455,   241,   635,    -1,   239,   240,
     455,   241,   635,   225,    42,   636,    43,    -1,   645,    -1,
     312,    -1,   314,    -1,   637,    -1,   636,    35,   637,    -1,
       9,   523,   560,    10,   455,    -1,   639,    -1,   640,    -1,
     646,    55,    56,    -1,   193,   448,   560,   452,    -1,   517,
      42,    43,    -1,   517,    42,   529,    43,    -1,   193,    42,
      43,    -1,   193,    42,    43,   152,   561,    -1,   193,    42,
     643,    43,   152,   561,    -1,   561,    -1,   643,    35,   561,
      -1,    42,   563,    43,    -1,   646,    -1,    59,    -1,    60,
      -1,    61,    -1,   191,    -1,   174,    -1,   153,    -1,    62,
      -1,    63,    -1,    64,    -1,    65,    -1,    66,    -1,   149,
      -1,    67,    -1,   193,    -1,   123,    -1,    68,    -1,    69,
      -1,    70,    -1,    71,    -1,    72,    -1,    73,    -1,    74,
      -1,    75,    -1,    76,    -1,    77,    -1,    78,    -1,    79,
      -1,    80,    -1,    81,    -1,    82,    -1,    83,    -1,    84,
      -1,    85,    -1,    86,    -1,    87,    -1,    88,    -1,    89,
      -1,    90,    -1,    92,    -1,    93,    -1,    94,    -1,    95,
      -1,    96,    -1,    97,    -1,    98,    -1,    99,    -1,   100,
      -1,   101,    -1,   102,    -1,   103,    -1,   104,    -1,   105,
      -1,   109,    -1,   110,    -1,   111,    -1,   112,    -1,   113,
      -1,   114,    -1,   115,    -1,   127,    -1,   132,    -1,   133,
      -1,   134,    -1,   135,    -1,   136,    -1,   137,    -1,   138,
      -1,   139,    -1,   140,    -1,   141,    -1,   142,    -1,   143,
      -1,   144,    -1,   145,    -1,   146,    -1,   147,    -1,   148,
      -1,   150,    -1,   152,    -1,   154,    -1,   155,    -1,   156,
      -1,   157,    -1,   158,    -1,   159,    -1,   160,    -1,   161,
      -1,   162,    -1,   163,    -1,   164,    -1,   165,    -1,   166,
      -1,   167,    -1,   168,    -1,   169,    -1,   170,    -1,   171,
      -1,   172,    -1,   173,    -1,   175,    -1,   176,    -1,   177,
      -1,   178,    -1,   179,    -1,   180,    -1,   181,    -1,   182,
      -1,   183,    -1,   184,    -1,   185,    -1,   187,    -1,   188,
      -1,   189,    -1,   192,    -1,   194,    -1,   195,    -1,   196,
      -1,   197,    -1,   198,    -1,   199,    -1,   200,    -1,   201,
      -1,   202,    -1,   203,    -1,   204,    -1,   205,    -1,   206,
      -1,   207,    -1,   208,    -1,   209,    -1,   210,    -1,   211,
      -1,   212,    -1,   213,    -1,   214,    -1,   215,    -1,   216,
      -1,   217,    -1,   218,    -1,   219,    -1,   220,    -1,   221,
      -1,   222,    -1,   223,    -1,   224,    -1,   225,    -1,   227,
      -1,   228,    -1,   229,    -1,   230,    -1,   231,    -1,   232,
      -1,   121,    -1,   122,    -1,   233,    -1,   234,    -1,   235,
      -1,   236,    -1,   237,    -1,   238,    -1,   239,    -1,   240,
      -1,   241,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   747,   747,   749,   750,   751,   752,   754,   761,   791,
     794,   815,   817,   821,   828,   830,   831,   832,   833,   834,
     835,   839,   858,   862,   867,   874,   879,   884,   893,   896,
     902,   907,   914,   914,   914,   917,   930,   934,   941,   946,
     955,   958,   966,   993,   997,  1004,  1010,  1015,  1020,  1029,
    1053,  1057,  1064,  1069,  1074,  1079,  1090,  1108,  1112,  1119,
    1124,  1132,  1157,  1160,  1163,  1166,  1169,  1172,  1175,  1178,
    1181,  1184,  1187,  1190,  1193,  1196,  1199,  1202,  1205,  1208,
    1214,  1215,  1216,  1217,  1218,  1230,  1233,  1238,  1241,  1246,
    1251,  1259,  1267,  1278,  1289,  1292,  1299,  1309,  1312,  1317,
    1328,  1331,  1336,  1341,  1349,  1350,  1351,  1352,  1353,  1354,
    1355,  1356,  1357,  1358,  1359,  1360,  1361,  1362,  1363,  1364,
    1365,  1366,  1370,  1388,  1392,  1397,  1406,  1412,  1412,  1415,
    1435,  1439,  1444,  1452,  1457,  1461,  1469,  1472,  1480,  1493,
    1497,  1508,  1511,  1519,  1537,  1541,  1548,  1553,  1558,  1566,
    1572,  1580,  1588,  1595,  1609,  1613,  1627,  1636,  1644,  1647,
    1654,  1689,  1693,  1698,  1704,  1714,  1717,  1725,  1728,  1735,
    1753,  1757,  1764,  1769,  1777,  1795,  1799,  1807,  1829,  1833,
    1840,  1848,  1857,  1865,  1902,  1906,  1913,  1920,  1929,  1938,
    1965,  1969,  1976,  1988,  2014,  2018,  2025,  2035,  2047,  2051,
    2056,  2065,  2078,  2082,  2087,  2096,  2110,  2114,  2127,  2132,
    2144,  2148,  2156,  2169,  2177,  2190,  2197,  2211,  2211,  2214,
    2214,  2236,  2240,  2244,  2262,  2280,  2290,  2304,  2312,  2318,
    2324,  2330,  2342,  2345,  2359,  2360,  2361,  2362,  2366,  2369,
    2373,  2375,  2379,  2383,  2387,  2391,  2395,  2401,  2407,  2421,
    2422,  2423,  2424,  2425,  2426,  2427,  2433,  2437,  2441,  2445,
    2451,  2451,  2458,  2459,  2460,  2461,  2465,  2468,  2472,  2474,
    2478,  2482,  2486,  2490,  2494,  2500,  2506,  2510,  2521,  2522,
    2523,  2524,  2528,  2531,  2535,  2537,  2541,  2545,  2549,  2553,
    2557,  2563,  2569,  2580,  2581,  2582,  2583,  2588,  2593,  2615,
    2618,  2622,  2634,  2636,  2640,  2644,  2648,  2652,  2656,  2662,
    2668,  2672,  2686,  2687,  2688,  2689,  2690,  2691,  2692,  2693,
    2697,  2697,  2700,  2704,  2712,  2717,  2726,  2731,  2740,  2800,
    2807,  2812,  2821,  2826,  2835,  2845,  2849,  2857,  2861,  2869,
    2885,  2894,  2906,  2915,  2918,  2923,  2932,  2936,  2944,  2954,
    2966,  2973,  2977,  2985,  2990,  3000,  3004,  3008,  3012,  3019,
    3023,  3030,  3033,  3041,  3046,  3056,  3064,  3068,  3076,  3089,
    3104,  3111,  3112,  3113,  3114,  3115,  3116,  3117,  3118,  3119,
    3120,  3121,  3122,  3127,  3138,  3142,  3146,  3150,  3154,  3160,
    3166,  3167,  3173,  3180,  3184,  3188,  3200,  3203,  3214,  3217,
    3227,  3234,  3238,  3242,  3246,  3253,  3261,  3266,  3274,  3278,
    3284,  3291,  3306,  3323,  3326,  3330,  3338,  3346,  3350,  3359,
    3367,  3378,  3382,  3386,  3397,  3404,  3409,  3417,  3421,  3429,
    3433,  3441,  3449,  3458,  3463,  3472,  3482,  3486,  3490,  3490,
    3494,  3498,  3502,  3506,  3510,  3514,  3518,  3522,  3526,  3530,
    3534,  3538,  3542,  3547,  3551,  3555,  3560,  3564,  3569,  3573,
    3577,  3582,  3586,  3590,  3594,  3598,  3603,  3607,  3611,  3616,
    3620,  3624,  3629,  3633,  3638,  3643,  3648,  3652,  3657,  3661,
    3666,  3672,  3678,  3683,  3684,  3685,  3693,  3697,  3701,  3709,
    3716,  3725,  3725,  3729,  3749,  3750,  3758,  3759,  3765,  3786,
    3790,  3804,  3810,  3823,  3827,  3827,  3831,  3835,  3844,  3856,
    3867,  3871,  3875,  3879,  3883,  3887,  3891,  3895,  3903,  3911,
    3928,  3936,  3945,  3949,  3953,  3957,  3961,  3969,  3981,  3986,
    3991,  3998,  4003,  4010,  4017,  4028,  4038,  4041,  4052,  4053,
    4054,  4055,  4056,  4057,  4058,  4059,  4060,  4061,  4065,  4065,
    4068,  4068,  4068,  4072,  4079,  4083,  4087,  4095,  4103,  4113,
    4123,  4127,  4134,  4139,  4148,  4152,  4161,  4161,  4161,  4165,
    4171,  4180,  4180,  4185,  4188,  4197,  4208,  4208,  4213,  4217,
    4224,  4228,  4239,  4242,  4247,  4258,  4261,  4265,  4279,  4282,
    4287,  4298,  4301,  4305,  4323,  4326,  4331,  4336,  4343,  4356,
    4370,  4373,  4378,  4397,  4398,  4411,  4412,  4413,  4414,  4415,
    4416,  4421,  4429,  4437,  4441,  4449,  4453,  4462,  4470,  4478,
    4486,  4493,  4503,  4510,  4514,  4522,  4533,  4536,  4545,  4548,
    4558,  4565,  4575,  4576,  4578,  4580,  4586,  4590,  4594,  4595,
    4596,  4600,  4612,  4613,  4614,  4615,  4616,  4617,  4618,  4619,
    4620,  4625,  4633,  4637,  4642,  4651,  4659,  4667,  4671,  4675,
    4683,  4687,  4691,  4699,  4700,  4708,  4716,  4721,  4725,  4729,
    4733,  4742,  4743,  4751,  4758,  4761,  4764,  4767,  4771,  4783,
    4795,  4811,  4814,  4818,  4828,  4839,  4844,  4855,  4860,  4864,
    4869,  4873,  4878,  4882,  4887,  4891,  4895,  4899,  4903,  4917,
    4921,  4925,  4929,  4934,  4938,  4942,  4946,  4950,  4964,  4965,
    4966,  4967,  4968,  4969,  4970,  4978,  4982,  4986,  4990,  5001,
    5005,  5009,  5013,  5021,  5025,  5035,  5039,  5043,  5047,  5051,
    5059,  5061,  5068,  5072,  5076,  5080,  5090,  5094,  5098,  5106,
    5108,  5116,  5120,  5127,  5131,  5139,  5143,  5151,  5159,  5163,
    5172,  5175,  5179,  5183,  5187,  5191,  5202,  5208,  5214,  5220,
    5230,  5234,  5238,  5246,  5250,  5258,  5266,  5271,  5276,  5288,
    5292,  5296,  5300,  5304,  5310,  5310,  5315,  5321,  5321,  5326,
    5330,  5340,  5348,  5355,  5360,  5368,  5377,  5389,  5401,  5414,
    5425,  5434,  5446,  5450,  5454,  5459,  5464,  5469,  5478,  5481,
    5489,  5493,  5501,  5505,  5513,  5518,  5527,  5531,  5535,  5543,
    5547,  5555,  5559,  5563,  5567,  5575,  5579,  5583,  5591,  5596,
    5606,  5619,  5619,  5623,  5631,  5640,  5645,  5654,  5658,  5662,
    5669,  5674,  5683,  5692,  5692,  5692,  5692,  5692,  5692,  5692,
    5692,  5693,  5693,  5693,  5693,  5693,  5693,  5694,  5698,  5698,
    5698,  5698,  5698,  5698,  5698,  5698,  5698,  5698,  5699,  5699,
    5699,  5699,  5699,  5699,  5699,  5700,  5700,  5700,  5700,  5700,
    5700,  5700,  5700,  5701,  5701,  5701,  5701,  5701,  5701,  5701,
    5701,  5701,  5701,  5701,  5702,  5702,  5702,  5702,  5702,  5702,
    5702,  5702,  5702,  5702,  5702,  5702,  5703,  5703,  5703,  5703,
    5703,  5703,  5703,  5703,  5703,  5703,  5704,  5704,  5704,  5704,
    5704,  5704,  5704,  5704,  5704,  5704,  5704,  5704,  5705,  5705,
    5705,  5705,  5705,  5705,  5705,  5705,  5705,  5705,  5705,  5705,
    5706,  5706,  5706,  5706,  5706,  5706,  5706,  5706,  5706,  5707,
    5707,  5707,  5707,  5707,  5707,  5707,  5707,  5708,  5708,  5708,
    5708,  5708,  5708,  5708,  5708,  5708,  5709,  5709,  5709,  5709,
    5709,  5709,  5709,  5709,  5709,  5710,  5710,  5710,  5710,  5710,
    5710,  5710,  5710,  5710,  5710,  5711,  5711,  5711,  5711,  5711,
    5711,  5711,  5711,  5711,  5711,  5711,  5712,  5712,  5712,  5712,
    5712,  5712,  5712,  5712,  5712,  5712,  5712
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"<end of file>\"", "error", "$undefined", "\"<Language: XPath2>\"",
  "\"<Language: XQuery>\"", "\"<Language: XQuery Fulltext>\"",
  "\"<Language: XQuery Update>\"",
  "\"<Language: XQuery Fulltext Update>\"", "\"<Language: XSLT 2.0>\"",
  "\"$\"", "\":=\"", "\"*\"", "\"|\"", "\"!=\"", "\"<=\"",
  "\"> (comparison operator)\"", "\">=\"", "\"<<\"", "\">>\"", "\"::\"",
  "\"@\"", "\"[\"", "\"]\"", "\"?\"", "\"||\"", "\"&&\"", "\"<\"",
  "\"> (start tag close)\"", "\"> (end tag close)\"", "\"<?\"", "\"<!--\"",
  "\"-->\"", "\"//\"", "\"</\"", "\"(#\"", "\",\"", "\"+\"", "\"-\"",
  "\"/\"", "\"= (comparison operator)\"", "\".\"", "\"..\"", "\"(\"",
  "\")\"", "\"/>\"", "\"=\"", "\"\\\" (open)\"", "\"\\\" (close)\"",
  "\"' (open)\"", "\"' (close)\"", "\"{\"", "\"{ (expression enclosure)\"",
  "\"}\"", "\";\"", "\"!\"", "\"#\"", "\"<integer literal>\"",
  "\"<decimal literal>\"", "\"<double literal>\"", "\"attribute\"",
  "\"comment\"", "\"document-node\"", "\"node\"",
  "\"processing-instruction\"", "\"schema-attribute\"",
  "\"schema-element\"", "\"text\"", "\"empty-sequence\"",
  "\"boundary-space\"", "\"ft-option\"", "\"base-uri\"", "\"lax\"",
  "\"strict\"", "\"idiv\"", "\"child\"", "\"descendant\"", "\"self\"",
  "\"descendant-or-self\"", "\"following-sibling\"", "\"following\"",
  "\"parent\"", "\"ancestor\"", "\"preceding-sibling\"", "\"preceding\"",
  "\"ancestor-of-self\"", "\"document\"", "\"not\"", "\"sensitive\"",
  "\"insensitive\"", "\"diacritics\"", "\"without\"",
  "\"without (followed by content)\"", "\"stemming\"", "\"thesaurus\"",
  "\"stop\"", "\"wildcards\"", "\"entire\"", "\"content\"", "\"word\"",
  "\"type\"", "\"start\"", "\"end\"", "\"most\"", "\"skip\"", "\"copy\"",
  "\"value\"", "\"<whitespace>\"", "\"<processing instruction content>\"",
  "\"<xml comment content>\"", "\"eq\"", "\"ne\"", "\"lt\"", "\"le\"",
  "\"gt\"", "\"ge\"", "\"at\"", "\"<quot attribute content>\"",
  "\"<apos attribute content>\"", "\"<whitespace element content>\"",
  "\"<element content>\"", "\"at (followed by least or most)\"",
  "\"ordered\"", "\"unordered\"", "\"<qualified name>\"",
  "\"<xmlns qualified name>\"",
  "\"<computed constructor qualified name>\"", "\"'...'\"", "\"variable\"",
  "\"<NCName>:*\"", "\"*:<NCName>\"",
  "\"<processing instruction target>\"", "\"<pragma content>\"",
  "\"return\"", "\"for\"", "\"in\"", "\"let\"", "\"where\"", "\"by\"",
  "\"order\"", "\"stable\"", "\"ascending\"", "\"descending\"",
  "\"empty\"", "\"greatest\"", "\"least\"", "\"collation\"", "\"some\"",
  "\"every\"", "\"satisfies\"", "\"typeswitch\"", "\"case\"",
  "\"case (followed by (in)sensitive)\"", "\"as\"", "\"if\"", "\"then\"",
  "\"else\"", "\"or\"", "\"and\"", "\"instance\"", "\"of\"",
  "\"castable\"", "\"to\"", "\"div\"", "\"mod\"", "\"union\"",
  "\"intersect\"", "\"except\"", "\"validate\"", "\"cast\"", "\"treat\"",
  "\"is\"", "\"preserve\"", "\"strip\"", "\"namespace\"", "\"item\"",
  "\"external\"", "\"encoding\"", "\"no-preserve\"", "\"inherit\"",
  "\"no-inherit\"", "\"declare\"", "\"construction\"", "\"ordering\"",
  "\"default\"", "\"copy-namespaces\"", "\"option\"", "\"xquery\"",
  "\"version\"", "\"import\"", "\"schema\"", "\"module\"", "\"element\"",
  "\"function\"", "\"function (ext)\"", "\"score\"", "\"ftcontains\"",
  "\"weight\"", "\"window\"", "\"distance\"", "\"occurs\"", "\"times\"",
  "\"same\"", "\"different\"", "\"lowercase\"", "\"uppercase\"",
  "\"relationship\"", "\"levels\"", "\"language\"", "\"any\"", "\"all\"",
  "\"phrase\"", "\"exactly\"", "\"from\"", "\"words\"", "\"sentences\"",
  "\"paragraphs\"", "\"sentence\"", "\"paragraph\"", "\"replace\"",
  "\"modify\"", "\"first\"", "\"insert\"", "\"before\"", "\"after\"",
  "\"revalidation\"", "\"with\"", "\"with (ft option)\"", "\"nodes\"",
  "\"rename\"", "\"last\"", "\"delete\"", "\"into\"", "\"updating\"",
  "\"id\"", "\"key\"", "\"template\"", "\"matches\"", "\"name\"",
  "\"call\"", "\"apply\"", "\"templates\"", "\"mode\"",
  "\"<XSLT end element>\"", "\"<xsl:stylesheet...\"",
  "\"<xsl:template...\"", "\"<xsl:value-of...\"", "\"<xsl:text...\"",
  "\"<xsl:apply-templates...\"", "\"<xsl:call-template...\"",
  "\"<xsl:with-param...\"", "\"<xsl:sequence...\"", "\"<xsl:param...\"",
  "\"<xsl:function...\"", "\"<xsl:choose...\"", "\"<xsl:when...\"",
  "\"<xsl:otherwise...\"", "\"<xsl:if...\"", "\"<xsl:variable...\"",
  "\"<xsl:comment...\"", "\"<xsl:processing-instruction...\"",
  "\"<xsl:document...\"", "\"<xsl:attribute...\"", "\"<xsl:namespace...\"",
  "\"<xsl:element...\"", "\"<xsl:analyze-string...\"",
  "\"<xsl:matching-substring...\"", "\"<xsl:non-matching-substring...\"",
  "\"<xsl:copy-of...\"", "\"<xsl:copy...\"", "\"<xsl:for-each...\"",
  "\"<xsl:output...\"", "\"<xsl:import-schema...\"", "\"version='...'\"",
  "\"mode='...'\"", "\"name='...'\"", "\"doctype-public='...'\"",
  "\"doctype-system='...'\"", "\"encoding='...'\"", "\"media-type='...'\"",
  "\"normalization-form='...'\"", "\"standalone='...'\"",
  "\"exclude-result-prefixes='...'\"", "\"namespace='...' (string)\"",
  "\"schema-location='...'\"", "\"tunnel='...'\"", "\"required='...'\"",
  "\"override='...'\"", "\"copy-namespaces='...'\"",
  "\"inherit-namespaces='...'\"", "\"byte-order-mark='...'\"",
  "\"escape-uri-attributes='...'\"", "\"include-content-type='...'\"",
  "\"indent='...'\"", "\"omit-xml-declaration='...'\"",
  "\"undeclare-prefixes='...'\"", "\"match='...'\"", "\"as='...'\"",
  "\"select='...'\"", "\"priority='...'\"", "\"test='...'\"",
  "\"separator='...'\"", "\"namespace='...' (attribute value template)\"",
  "\"regex='...'\"", "\"flags='...'\"", "\"method='...'\"",
  "\"cdata-section-elements='...'\"", "\"use-character-maps='...'\"",
  "\"<XSLT element name>\"", "\"<XSLT XMLNS attr>\"",
  "\"<XSLT attr name>\"", "\"<XSLT text node>\"",
  "\"<XSLT whitespace text node>\"", "\"#default\"", "\"#all\"",
  "\"#current\"", "\"xml\"", "\"html\"", "\"xhtml\"", "$accept",
  "SelectLanguage", "Start_XSLT", "Stylesheet_XSLT",
  "StylesheetAttrs_XSLT", "StylesheetContent_XSLT", "Template_XSLT",
  "TemplateAttrs_XSLT", "TemplateModes_XSLT", "Number_XSLT",
  "Function_XSLT", "FunctionAttrs_XSLT", "ParamList_XSLT", "Param_XSLT",
  "ParamAttrs_XSLT", "GlobalParam_XSLT", "GlobalParamAttrs_XSLT",
  "GlobalVariable_XSLT", "GlobalVariableAttrs_XSLT", "Output_XSLT",
  "OutputAttrs_XSLT", "OutputMethod_XSLT", "QNames_XSLT",
  "ImportSchema_XSLT", "ImportSchemaAttrs_XSLT",
  "LiteralResultElement_XSLT", "LiteralResultElementAttrs_XSLT",
  "AttrValueTemplate_XSLT", "SequenceConstructor_XSLT", "Instruction_XSLT",
  "ValueOf_XSLT", "ValueOfAttrs_XSLT", "Text_XSLT", "TextNode_XSLT",
  "ApplyTemplates_XSLT", "ApplyTemplatesAttrs_XSLT",
  "ApplyTemplatesMode_XSLT", "ApplyTemplatesContent_XSLT",
  "CallTemplate_XSLT", "CallTemplateAttrs_XSLT",
  "CallTemplateContent_XSLT", "WithParam_XSLT", "WithParamAttrs_XSLT",
  "Sequence_XSLT", "SequenceAttrs_XSLT", "If_XSLT", "IfAttrs_XSLT",
  "Choose_XSLT", "WhenList_XSLT", "When_XSLT", "WhenAttrs_XSLT",
  "Otherwise_XSLT", "AnalyzeString_XSLT", "AnalyzeStringAttrs_XSLT",
  "MatchingSubstring_XSLT", "NonMatchingSubstring_XSLT", "Variable_XSLT",
  "VariableAttrs_XSLT", "Comment_XSLT", "CommentAttrs_XSLT", "PI_XSLT",
  "PIAttrs_XSLT", "Document_XSLT", "DocumentAttrs_XSLT", "Attribute_XSLT",
  "AttributeAttrs_XSLT", "Namespace_XSLT", "NamespaceAttrs_XSLT",
  "Element_XSLT", "ElementAttrs_XSLT", "CopyOf_XSLT", "CopyOfAttrs_XSLT",
  "Copy_XSLT", "CopyAttrs_XSLT", "ForEach_XSLT", "ForEachAttrs_XSLT",
  "Pattern_XSLT", "PathPattern_XSLT", "PathPatternStart_XSLT",
  "IdKeyPattern_XSLT", "IdValue_XSLT", "KeyValue_XSLT",
  "RelativePathPattern_XSLT", "PatternStep_XSLT", "PatternAxis_XSLT",
  "PatternStepPredicateList_XSLT", "Module_XQ", "MainModule_XQ",
  "LibraryModule_XQ", "Prolog_XQ", "Setter_XQ", "FunctionDecl_XQ",
  "FunctionKeyword", "Module_XQF", "MainModule_XQF", "LibraryModule_XQF",
  "Prolog_XQF", "Module_XQU", "MainModule_XQU", "LibraryModule_XQU",
  "Prolog_XQU", "Module", "VersionDecl", "MainModule", "LibraryModule",
  "ModuleDecl", "Prolog", "Setter", "Import", "Separator", "NamespaceDecl",
  "BoundarySpaceDecl", "DefaultNamespaceDecl", "OptionDecl",
  "FTOptionDecl", "OrderingModeDecl", "EmptyOrderDecl",
  "CopyNamespacesDecl", "PreserveMode", "InheritMode",
  "DefaultCollationDecl", "BaseURIDecl", "SchemaImport",
  "ResourceLocations", "SchemaPrefix", "ModuleImport", "VarDecl",
  "VarDeclValue", "ConstructionDecl", "FunctionDecl", "FunctionParamList",
  "FunctionDeclUpdating", "ParamList", "Param", "EnclosedExpr",
  "QueryBody", "Expr", "ExprSingle", "FLWORExpr", "FLWORTuples",
  "FlworExprForLetList", "ForClause", "ForBindingList", "ForBinding",
  "PositionalVar", "FTScoreVar", "LetClause", "LetBindingList",
  "LetBinding", "WhereClause", "OrderByClause", "OrderSpecList",
  "OrderSpec", "OrderExpr", "OrderDirection", "EmptyHandling",
  "QuantifiedExpr", "QuantifyBindingList", "QuantifyBinding",
  "TypeswitchExpr", "CaseClauseList", "DefaultCase", "CaseClause",
  "IfExpr", "OrExpr", "AndExpr", "ComparisonExpr", "@1", "FTContainsExpr",
  "RangeExpr", "AdditiveExpr", "MultiplicativeExpr", "UnionExpr",
  "IntersectExceptExpr", "InstanceofExpr", "TreatExpr", "CastableExpr",
  "CastExpr", "UnaryExpr", "ValueExpr", "ValidateExpr", "ExtensionExpr",
  "PragmaList", "Pragma", "PragmaContents", "PathExpr", "LeadingSlash",
  "RelativePathExpr", "StepExpr", "AxisStep", "ForwardStep", "ForwardAxis",
  "AbbrevForwardStep", "ReverseStep", "ReverseAxis", "AbbrevReverseStep",
  "NodeTest", "NameTest", "Wildcard", "FilterExpr", "PredicateList",
  "PrimaryExpr", "Literal", "NumericLiteral", "VarRef", "VarName",
  "ParenthesizedExpr", "ContextItemExpr", "OrderedExpr", "UnorderedExpr",
  "FunctionCall", "FunctionCallArgumentList", "Constructor",
  "DirectConstructor", "DirElemConstructor", "DirElemConstructorQName",
  "DirAttributeList", "OptionalWhitespace", "DirAttributeValue",
  "LiteralDirAttributeValue", "QuotAttrValueContent",
  "LiteralQuotAttrValueContent", "AposAttrValueContent",
  "LiteralAposAttrValueContent", "DirElementContent",
  "DirCommentConstructor", "DirCommentContents", "DirPIConstructor",
  "DirPIContents", "ComputedConstructor", "CompDocConstructor",
  "CompElemConstructor", "CompConstructorName", "ContentExpr",
  "CompAttrConstructor", "CompTextConstructor", "CompCommentConstructor",
  "CompPIConstructor", "CompPINCName", "CompPIConstructorContent",
  "SingleType", "SingleTypeOccurrence", "TypeDeclaration", "SequenceType",
  "OccurrenceIndicator", "ItemType", "AtomicType", "KindTest",
  "AnyKindTest", "DocumentTest", "TextTest", "CommentTest", "PITest",
  "AttributeTest", "AttribNameOrWildcard", "SchemaAttributeTest",
  "AttributeDeclaration", "ElementTest", "ElementNameOrWildcard",
  "SchemaElementTest", "ElementDeclaration", "AttributeName",
  "ElementName", "TypeName", "URILiteral", "FTSelection",
  "FTSelectionOptions", "FTOr", "FTAnd", "FTMildnot", "FTUnaryNot",
  "FTWordsSelection", "FTWords", "FTProximity", "FTMatchOption",
  "FTCaseOption", "FTDiacriticsOption", "FTStemOption",
  "FTThesaurusOption", "FTThesaurusIDList", "FTThesaurusID",
  "FTStopwordOption", "FTInclExclStringLiteralList", "FTRefOrList",
  "FTRefOrListStringList", "FTInclExclStringLiteral", "FTLanguageOption",
  "FTWildCardOption", "FTAnyallOption", "FTRange", "FTUnit", "FTBigUnit",
  "FTIgnoreOption", "RevalidationDecl", "InsertExpr", "InsertExprBegin",
  "DeleteExpr", "DeleteExprBegin", "ReplaceExpr", "RenameExpr",
  "TransformExpr", "TransformBindingList", "TransformBinding",
  "IntegerLiteral", "DecimalLiteral", "DoubleLiteral", "StringLiteral",
  "NCName", "QName", "TemplateDecl", "TemplateSequenceType",
  "TemplateParamList", "TemplateDeclModesSection", "TemplateDeclModes",
  "TemplateDeclMode", "CallTemplateExpr", "ApplyTemplatesExpr",
  "ApplyTemplatesMode", "TemplateArgumentList", "TemplateArgument",
  "FunctionRef", "LiteralFunctionRef", "InlineFunction",
  "DereferencedFunctionCall", "FunctionType", "FunctionTypeArguments",
  "ParenthesizedItemType", "QNameValue", "FunctionName", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   472,   473,   474,
     475,   476,   477,   478,   479,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   318,   319,   319,   319,   319,   319,   319,   320,   321,
     321,   322,   322,   322,   323,   323,   323,   323,   323,   323,
     323,   324,   325,   325,   325,   325,   325,   325,   326,   326,
     326,   326,   327,   327,   327,   328,   329,   329,   329,   329,
     330,   330,   331,   332,   332,   332,   332,   332,   332,   333,
     334,   334,   334,   334,   334,   334,   335,   336,   336,   336,
     336,   337,   338,   338,   338,   338,   338,   338,   338,   338,
     338,   338,   338,   338,   338,   338,   338,   338,   338,   338,
     339,   339,   339,   339,   339,   340,   340,   341,   342,   342,
     342,   342,   342,   343,   344,   344,   344,   345,   345,   345,
     346,   346,   346,   346,   347,   347,   347,   347,   347,   347,
     347,   347,   347,   347,   347,   347,   347,   347,   347,   347,
     347,   347,   348,   349,   349,   349,   350,   351,   351,   352,
     353,   353,   353,   354,   354,   354,   355,   355,   356,   357,
     357,   358,   358,   359,   360,   360,   360,   360,   360,   361,
     362,   363,   364,   365,   366,   366,   367,   368,   369,   369,
     370,   371,   371,   371,   371,   372,   372,   373,   373,   374,
     375,   375,   375,   375,   376,   377,   377,   378,   379,   379,
     379,   380,   381,   382,   383,   383,   383,   383,   383,   384,
     385,   385,   385,   386,   387,   387,   387,   388,   389,   389,
     389,   390,   391,   391,   391,   392,   393,   393,   394,   394,
     395,   395,   395,   396,   396,   397,   397,   398,   398,   399,
     399,   400,   400,   400,   400,   400,   400,   401,   402,   402,
     402,   402,   403,   403,   404,   404,   404,   404,   405,   406,
     407,   407,   407,   407,   407,   407,   407,   407,   407,   408,
     408,   408,   408,   408,   408,   408,   409,   409,   409,   409,
     410,   410,   411,   411,   411,   411,   412,   413,   414,   414,
     414,   414,   414,   414,   414,   414,   414,   414,   415,   415,
     415,   415,   416,   417,   418,   418,   418,   418,   418,   418,
     418,   418,   418,   419,   419,   419,   419,   420,   420,   421,
     422,   423,   424,   424,   424,   424,   424,   424,   424,   424,
     424,   424,   425,   425,   425,   425,   425,   425,   425,   425,
     426,   426,   427,   428,   429,   429,   430,   430,   431,   432,
     433,   433,   434,   434,   435,   436,   436,   437,   437,   438,
     439,   440,   440,   441,   441,   441,   442,   442,   443,   443,
     444,   445,   445,   446,   446,   447,   447,   447,   447,   448,
     448,   449,   449,   450,   450,   451,   452,   452,   453,   454,
     454,   455,   455,   455,   455,   455,   455,   455,   455,   455,
     455,   455,   455,   456,   457,   457,   457,   457,   458,   458,
     458,   458,   459,   460,   460,   461,   462,   462,   463,   463,
     464,   465,   465,   466,   466,   467,   468,   468,   469,   469,
     470,   470,   471,   472,   472,   472,   473,   473,   473,   474,
     474,   475,   475,   476,   477,   478,   478,   479,   479,   480,
     480,   481,   482,   482,   483,   483,   484,   484,   485,   484,
     484,   484,   484,   484,   484,   484,   484,   484,   484,   484,
     484,   484,   484,   486,   486,   486,   487,   487,   488,   488,
     488,   489,   489,   489,   489,   489,   490,   490,   490,   491,
     491,   491,   492,   492,   493,   493,   494,   494,   495,   495,
     496,   496,   496,   497,   497,   497,   498,   498,   498,   499,
     499,   500,   500,   501,   502,   502,   503,   503,   503,   503,
     504,   505,   505,   505,   506,   506,   507,   507,   508,   508,
     509,   509,   509,   509,   509,   509,   509,   509,   510,   510,
     511,   511,   512,   512,   512,   512,   512,   513,   514,   514,
     515,   515,   516,   516,   516,   517,   518,   518,   519,   519,
     519,   519,   519,   519,   519,   519,   519,   519,   520,   520,
     521,   521,   521,   522,   523,   524,   524,   525,   526,   527,
     528,   528,   529,   529,   530,   530,   531,   531,   531,   532,
     532,   533,   533,   534,   534,   534,   535,   535,   536,   536,
     537,   537,   538,   538,   538,   539,   539,   539,   540,   540,
     540,   541,   541,   541,   542,   542,   542,   542,   542,   543,
     544,   544,   545,   546,   546,   547,   547,   547,   547,   547,
     547,   548,   549,   550,   550,   551,   551,   552,   553,   554,
     555,   556,   556,   557,   557,   558,   559,   559,   560,   560,
     561,   561,   562,   562,   562,   562,   563,   563,   563,   563,
     563,   564,   565,   565,   565,   565,   565,   565,   565,   565,
     565,   566,   567,   567,   567,   568,   569,   570,   570,   570,
     571,   571,   571,   572,   572,   573,   574,   575,   575,   575,
     575,   576,   576,   577,   578,   579,   580,   581,   582,   583,
     583,   584,   584,   584,   585,   585,   586,   586,   587,   587,
     588,   588,   589,   589,   590,   590,   590,   590,   590,   591,
     591,   591,   591,   591,   591,   591,   591,   591,   592,   592,
     592,   592,   592,   592,   592,   593,   593,   593,   593,   594,
     594,   594,   594,   595,   595,   596,   596,   596,   596,   596,
     597,   597,   598,   598,   598,   598,   599,   599,   599,   600,
     600,   601,   601,   602,   602,   603,   603,   604,   605,   605,
     606,   606,   606,   606,   606,   606,   607,   607,   607,   607,
     608,   608,   608,   609,   609,   610,   611,   611,   611,   612,
     612,   612,   612,   612,   613,   613,   614,   615,   615,   616,
     616,   617,   618,   619,   619,   620,   621,   622,   623,   624,
     625,   626,   627,   627,   627,   627,   627,   627,   628,   628,
     629,   629,   630,   630,   631,   631,   632,   632,   632,   633,
     633,   634,   634,   634,   634,   635,   635,   635,   636,   636,
     637,   638,   638,   639,   640,   641,   641,   642,   642,   642,
     643,   643,   644,   645,   645,   645,   645,   645,   645,   645,
     645,   645,   645,   645,   645,   645,   645,   645,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646,   646,   646,   646,
     646,   646,   646,   646,   646,   646,   646
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     1,     4,
       1,     0,     2,     2,     0,     2,     2,     2,     2,     2,
       2,     4,     1,     3,     2,     3,     3,     3,     0,     2,
       2,     2,     1,     1,     1,     4,     1,     2,     3,     2,
       0,     2,     3,     1,     2,     3,     3,     2,     2,     3,
       1,     2,     3,     3,     2,     2,     3,     1,     2,     3,
       3,     2,     1,     2,     3,     2,     3,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     3,     2,
       1,     1,     1,     1,     1,     1,     2,     2,     1,     2,
       2,     3,     3,     4,     0,     3,     2,     0,     2,     2,
       0,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     3,     3,     1,     1,     3,
       1,     3,     3,     1,     1,     1,     0,     2,     3,     1,
       2,     0,     2,     3,     1,     2,     3,     3,     2,     2,
       3,     3,     3,     4,     1,     2,     3,     3,     0,     3,
       4,     1,     3,     3,     3,     0,     3,     0,     3,     4,
       1,     2,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     3,     1,     3,     1,     3,     3,     3,     3,     3,
       1,     3,     3,     3,     1,     3,     3,     2,     1,     3,
       2,     3,     1,     2,     2,     3,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     4,     6,     1,     1,     1,
       1,     1,     2,     3,     3,     3,     3,     2,     1,     3,
       3,     2,     0,     4,     2,     2,     1,     1,     2,     2,
       0,     3,     3,     3,     3,     3,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     5,     7,     5,     7,
       1,     1,     2,     2,     1,     1,     2,     2,     0,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       1,     1,     2,     2,     0,     3,     3,     3,     3,     3,
       3,     3,     3,     2,     2,     1,     1,     4,     6,     2,
       2,     6,     0,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     5,     3,     3,     5,     5,     4,     3,
       3,     3,     5,     5,     5,     1,     1,     1,     1,     4,
       3,     5,     4,     0,     2,     3,     3,     3,     7,     4,
       6,     2,     1,     3,     3,     6,     8,     6,     8,     2,
       3,     0,     1,     3,     1,     3,     3,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     2,     2,     1,     2,     2,
       1,     1,     2,     3,     1,     7,     0,     3,     0,     3,
       2,     3,     1,     6,     5,     2,     3,     4,     3,     1,
       3,     5,     1,     0,     1,     1,     0,     2,     2,     4,
       4,     3,     1,     5,     6,     2,     1,     5,     3,     4,
       7,     8,     3,     1,     3,     1,     3,     3,     0,     4,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     3,     4,     1,     3,     1,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     1,     4,     1,     4,     1,     4,     1,     4,     1,
       2,     2,     1,     1,     1,     1,     4,     5,     5,     3,
       4,     1,     2,     4,     1,     2,     1,     2,     2,     1,
       1,     3,     3,     1,     1,     1,     2,     2,     2,     1,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       2,     1,     2,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     0,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     3,     2,     1,     4,     4,
       3,     4,     3,     1,     1,     1,     1,     1,     1,     5,
      10,     1,     1,     0,     7,     7,     0,     1,     3,     3,
       3,     3,     0,     2,     2,     0,     2,     2,     0,     2,
       2,     0,     2,     2,     0,     2,     2,     2,     2,     3,
       0,     1,     3,     1,     2,     1,     1,     1,     1,     1,
       1,     4,     3,     1,     3,     2,     3,     3,     4,     4,
       3,     1,     3,     2,     3,     2,     0,     1,     0,     2,
       2,     3,     0,     1,     1,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     4,     4,     3,     3,     3,     4,     4,
       3,     4,     6,     1,     1,     4,     1,     3,     4,     6,
       7,     1,     1,     4,     1,     1,     1,     1,     1,     4,
       2,     0,     2,     2,     3,     1,     3,     1,     4,     1,
       2,     1,     3,     1,     2,     2,     2,     2,     4,     1,
       3,     3,     3,     2,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     3,     3,     6,     6,     2,
       0,     3,     2,     4,     4,     6,     5,     3,     5,     0,
       2,     2,     3,     1,     3,     2,     2,     2,     2,     2,
       0,     1,     2,     1,     2,     1,     2,     3,     3,     4,
       1,     1,     1,     1,     1,     3,     3,     3,     3,     6,
       6,     4,     4,     4,     2,     2,     2,     2,     2,     7,
       5,     5,     6,     3,     1,     4,     1,     1,     1,     1,
       1,     1,     5,     7,     8,    10,     6,     8,     0,     2,
       2,     3,     0,     2,     1,     3,     1,     1,     1,     3,
       7,     3,     7,     5,     9,     1,     1,     1,     1,     3,
       5,     1,     1,     3,     4,     3,     4,     3,     5,     6,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,     0,   240,   268,   284,   302,     0,     0,     0,   532,
       0,     0,     0,   600,     0,   576,     0,     0,   500,   557,
     527,     0,   786,   787,   788,   834,   835,   836,   840,   841,
     842,   843,   844,   846,   849,   850,   851,   852,   853,   854,
     855,   856,   857,   858,   859,   860,   861,   862,   863,   864,
     865,   866,   867,   868,   869,   870,   871,   872,   873,   874,
     875,   876,   877,   878,   879,   880,   881,   882,   883,   884,
     885,   886,   887,   888,   889,   890,   891,   892,   986,   987,
     848,   789,   893,   533,   534,   894,   895,   896,   897,   898,
     899,   900,   901,   902,   903,   904,   905,   906,   907,   908,
     909,   910,   845,   911,   912,   839,   913,   914,   915,   916,
     917,   918,   919,   920,   921,   922,   923,   924,   925,   926,
     927,   928,   929,   930,   931,   932,   838,   933,   934,   935,
     936,   937,   938,   939,   940,   941,   942,   943,   944,   945,
     946,   837,   947,   847,   948,   949,   950,   951,   952,   953,
     954,   955,   956,   957,   958,   959,   960,   961,   962,   963,
     964,   965,   966,   967,   968,   969,   970,   971,   972,   973,
     974,   975,   976,   977,   978,   979,   980,   981,   982,   983,
     984,   985,   988,   989,   990,   991,   992,   993,   994,   995,
     996,     2,   368,   370,   371,     0,   387,   390,   391,   372,
     373,   374,   380,   433,   435,   452,   455,   457,   460,   465,
     468,   471,   473,   475,   477,   479,   482,   483,   485,     0,
     491,   484,   496,   499,   503,   504,   536,     0,   509,   536,
       0,   521,   519,   529,   531,   505,   536,   538,   548,   539,
     540,   541,   544,   545,   542,   543,   564,   566,   567,   568,
     565,   605,   606,   607,   608,   609,   610,   528,   650,   642,
     649,   648,   647,   644,   646,   643,   645,   375,     0,   376,
       0,   378,   377,   379,   550,   551,   552,   549,   530,   381,
     382,   546,   821,   822,   547,   791,   833,     0,     0,     3,
     236,   237,     0,   240,   240,     4,   264,   265,     0,   268,
     268,     5,   280,   281,     0,   284,   284,     6,   302,   295,
     296,   302,     0,    11,    94,     7,     8,    10,     1,   834,
     835,   836,   840,   841,   842,   843,   844,   855,   856,   857,
     858,   859,   860,   861,   862,   863,   864,   865,   866,   884,
     986,   987,   895,   897,   908,   909,   845,   839,   926,   932,
     837,   847,   972,   975,   981,   983,   993,   994,   553,   554,
     833,   834,   835,   841,   844,   837,   518,   571,   572,   573,
       0,   601,     0,   498,   577,     0,   481,   480,   556,     0,
     512,     0,     0,   613,     0,     0,     0,     0,     0,     0,
       0,   621,     0,     0,     0,     0,     0,   510,   511,   513,
     514,   515,   516,   522,   523,   524,   525,   526,     0,     0,
       0,   784,     0,     0,     0,   392,   394,     0,     0,   400,
     402,     0,     0,   422,     0,     0,     0,     0,     0,     0,
     517,     0,     0,     0,   628,     0,     0,   774,   775,     0,
     777,   778,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   388,   389,   385,   386,     0,     0,     0,     0,     0,
       0,     0,     0,   438,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   492,
     497,     0,     0,   506,   508,   507,   520,     0,   535,     0,
     776,     0,     0,     0,     0,   938,   945,     0,     0,     0,
       0,   249,     0,     0,   253,   254,   255,   250,   251,   320,
     321,     0,   252,   238,     0,   234,   235,   239,   938,     0,
       0,     0,     0,     0,     0,     0,     0,   266,     0,   262,
     263,   267,   938,     0,     0,     0,   312,     0,     0,   316,
     317,   319,   313,   314,     0,   315,     0,   282,   318,     0,
     278,   279,   283,   293,   294,   300,   938,     0,     0,     0,
       0,     0,     0,     0,     0,   299,     0,    14,   100,   576,
       0,   603,   602,   599,     0,   555,   664,   660,     0,   663,
     675,     0,     0,   617,   656,     0,   652,     0,     0,     0,
       0,   651,   657,     0,     0,   790,     0,     0,   620,     0,
     666,     0,   674,   676,   655,     0,     0,     0,     0,     0,
       0,     0,   628,     0,   628,     0,     0,   628,     0,     0,
       0,     0,     0,     0,     0,     0,   672,   667,     0,   671,
     612,     0,   359,     0,   364,     0,     0,     0,     0,     0,
     809,   811,   369,   383,   405,     0,     0,   384,   432,   434,
     437,   440,   441,   442,   450,   451,     0,   436,   443,   444,
     445,   446,   447,   448,   449,     0,     0,     0,   750,   750,
     750,   750,   453,   681,   685,   687,   689,   691,   693,     0,
     458,   459,   456,   461,   463,   462,   464,   466,   467,   469,
     470,     0,     0,     0,     0,   489,     0,   502,   501,     0,
     825,   563,     0,     0,     0,     0,     0,   560,     0,   823,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   260,   261,   802,     0,     0,     0,   322,   241,   246,
     242,   243,   244,   248,   245,   247,     0,     0,     0,   269,
     274,   270,   271,   272,   276,   277,   273,   275,     0,     0,
     362,     0,   285,   286,   287,   288,   292,   289,   290,   291,
     361,   361,   303,   304,   305,   306,   310,   311,   307,   308,
     309,    12,    13,     0,    96,    97,     0,   577,     0,   604,
       0,   494,   493,     0,   661,   614,   615,     0,   619,   653,
     654,   659,   658,   622,   623,     0,   665,   673,   618,   611,
       0,   783,     0,   558,   559,   396,   393,   398,     0,   401,
       0,   421,   419,   420,     0,     0,   486,     0,     0,     0,
     668,   628,     0,   360,     0,   846,   838,   847,   629,   632,
     636,   638,   641,   639,   640,     0,     0,   824,     0,     0,
       0,     0,     0,     0,   412,   406,   409,   413,     0,   439,
       0,     0,   690,   751,   753,   755,   694,   695,   696,   697,
       0,   454,     0,   680,     0,     0,   472,   474,   476,   626,
     478,   490,     0,     0,   826,     0,     0,   773,   772,   771,
     561,     0,   297,     0,   324,   325,   678,   340,     0,     0,
     353,   354,   330,   331,     0,     0,     0,     0,   335,   336,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   343,
       0,   343,     0,     0,     0,   715,   716,     0,     0,   329,
     708,   709,   710,   711,   712,   713,   714,   767,   766,   768,
       0,     9,    22,    50,    36,    57,    62,    88,    15,    40,
      16,    40,    17,   100,    18,   100,    19,     0,    20,     0,
      95,    93,   123,     0,   130,   139,     0,     0,     0,   170,
     175,   178,   182,   184,   190,   194,   161,   198,   202,   206,
     127,   128,   102,   103,   104,   100,   105,   101,   106,   136,
     107,   141,   108,     0,   110,   100,   109,   118,   165,   111,
     100,   112,   100,   113,   100,   114,   100,   115,   100,   116,
     100,   117,   100,   119,     0,   120,   100,   121,   100,   576,
     576,   594,   569,   495,     0,   677,   616,   624,   785,     0,
       0,   398,     0,     0,     0,     0,     0,     0,   426,     0,
     487,   488,     0,   365,   363,     0,     0,     0,     0,   633,
     635,   634,   630,     0,     0,   780,     0,   781,     0,     0,
     816,   817,   813,   815,     0,   414,   415,   416,   407,   692,
     750,   752,   754,     0,   684,     0,     0,   699,     0,     0,
       0,     0,     0,     0,   683,   682,   686,     0,   627,   625,
     537,   562,     0,     0,     0,     0,   628,     0,     0,   339,
       0,     0,     0,   328,   802,   806,   807,   808,   803,   804,
       0,     0,     0,     0,   343,     0,   342,     0,   349,   721,
     722,   720,   724,   729,     0,   749,   717,   718,   747,   719,
     723,     0,     0,   748,     0,     0,    89,    90,    28,    24,
       0,     0,     0,   100,    37,    39,     0,   100,    51,    55,
      54,     0,     0,     0,    58,     0,     0,     0,    61,    79,
      63,    67,    68,    69,    73,    74,    76,    65,    70,    71,
      72,    75,    77,     0,     0,     0,    87,    99,    98,     0,
       0,     0,   158,   154,   100,     0,     0,    97,     0,     0,
       0,     0,   140,     0,   149,     0,   100,     0,    97,    97,
     167,   171,     0,     0,     0,     0,     0,    97,     0,     0,
       0,    97,     0,    97,    97,     0,    97,     0,     0,    97,
      97,     0,   197,   200,     0,   203,   204,     0,     0,     0,
       0,     0,     0,   662,   782,     0,     0,     0,     0,   404,
     423,     0,     0,     0,   424,   425,     0,     0,   669,   832,
     631,   637,   827,   830,     0,   366,   367,     0,     0,     0,
     818,     0,     0,   408,     0,   410,   698,   765,   707,   705,
     706,   679,     0,     0,     0,     0,     0,     0,   763,   764,
     703,   704,   688,   769,   770,   298,   301,     0,   323,   332,
     333,   326,   327,   337,   338,   334,     0,   792,   798,     0,
       0,     0,   214,   213,   834,   855,   988,   989,     0,   209,
       0,   212,   210,   221,   232,   228,     0,   258,   256,   346,
     347,   341,   344,     0,     0,   737,     0,     0,   726,   725,
       0,     0,     0,    91,    92,    26,    23,    27,    32,    33,
      34,    25,    43,    41,   100,     0,    38,     0,    53,    52,
      49,    60,    59,    56,    83,    84,    80,    81,    82,    64,
      85,    66,    78,   126,   150,     0,   100,   155,     0,     0,
     152,   124,   125,   122,   133,   134,   135,   132,   131,   129,
     144,   137,   100,   138,   142,   151,     0,   162,   163,   164,
     100,     0,   173,   172,   100,   176,   174,   179,   180,   177,
     181,   185,   187,   188,   186,   183,   191,   192,   189,   195,
     196,   193,   199,   201,   207,   205,   576,   576,     0,   598,
     597,   596,   595,   397,     0,   399,   403,     0,     0,     0,
       0,     0,   670,     0,     0,     0,   779,   628,     0,   810,
     812,     0,   417,   418,     0,   760,   761,   762,   700,     0,
       0,   756,     0,   701,   702,     0,   352,   350,   800,     0,
       0,     0,     0,   805,   231,     0,     0,     0,     0,     0,
     796,   798,   222,     0,     0,     0,     0,   227,     0,     0,
     345,   343,   730,   730,   732,     0,     0,   739,   739,     0,
     357,   355,    29,    30,    31,    44,    48,    47,     0,     0,
       0,    21,    35,    86,   157,     0,   153,   156,   145,   148,
       0,     0,     0,   166,     0,   160,   169,     0,     0,   576,
     395,     0,   429,     0,   428,   431,   828,   831,     0,     0,
     819,     0,   411,   758,   757,     0,   351,   801,   799,   793,
       0,   230,   229,     0,   218,   217,     0,   208,     0,   224,
     223,   226,   225,     0,   259,   257,   348,     0,     0,     0,
       0,     0,   743,   741,   736,   738,     0,    46,    45,    42,
     159,   147,   146,   143,   168,   582,   588,   574,   585,   591,
     575,     0,     0,     0,   829,     0,   814,   759,   794,   798,
     215,     0,   797,     0,     0,   728,   727,   733,   734,     0,
     742,     0,     0,   740,   358,   356,     0,     0,     0,     0,
     570,     0,   427,   820,     0,     0,   219,   220,   233,   731,
       0,   744,   745,   746,   578,   584,   583,   579,   590,   589,
     580,   587,   586,   581,   593,   592,   430,   795,   216,   735
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     7,   315,   316,   577,   783,   948,   949,  1335,  1341,
     950,   951,  1143,  1343,  1344,   952,   953,   954,   955,   956,
     957,  1359,  1361,   958,   959,   982,   578,   960,   786,   983,
     984,   985,   986,   987,   988,   989,  1377,  1191,   990,   991,
    1193,  1381,  1382,   992,   993,   994,   995,   996,  1182,  1183,
    1184,  1368,   997,   998,  1200,  1391,   999,  1000,  1001,  1002,
    1003,  1004,  1005,  1006,  1007,  1008,  1009,  1010,  1011,  1012,
    1013,  1014,  1015,  1016,  1017,  1018,  1308,  1309,  1310,  1311,
    1543,  1615,  1312,  1313,  1314,  1477,   289,   290,   291,   292,
     507,   508,   734,   295,   296,   297,   298,   301,   302,   303,
     304,   307,   293,   309,   310,   294,   312,   543,   509,   738,
     510,   511,   512,   513,   535,   514,   515,   516,   910,  1295,
     517,   518,   519,  1116,   918,   520,   521,  1457,   522,   556,
     434,   761,   643,   644,  1178,   191,   192,   193,   194,   195,
     196,   197,   415,   416,  1031,  1033,   198,   419,   420,   453,
     454,   855,   856,   857,  1067,  1265,   199,   422,   423,   200,
    1037,  1244,  1038,   201,   202,   203,   204,   666,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   792,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     493,   236,   237,   238,   239,   358,   240,   241,   242,   243,
     244,   712,   245,   246,   247,   369,   579,   375,  1577,  1580,
    1606,  1608,  1607,  1609,  1232,   248,   372,   249,   582,   250,
     251,   252,   384,   593,   253,   254,   255,   256,   392,   608,
     878,  1089,   646,   838,  1052,   839,   840,   257,   258,   259,
     260,   261,   262,   263,   588,   264,   609,   265,   638,   266,
     611,   589,   612,  1024,   897,   682,   873,   683,   684,   685,
     686,   687,   688,  1084,   929,   930,   931,   932,   933,  1557,
    1329,   934,  1564,  1487,  1561,  1603,   935,   936,   866,  1276,
    1448,  1280,   871,   558,   267,   268,   269,   270,   271,   272,
     273,   410,   411,   274,   275,   276,   277,   604,   278,   524,
    1461,  1298,   914,  1108,  1109,   279,   280,  1062,  1259,  1260,
     281,   282,   283,   284,   843,  1254,   844,   285,   286
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -1335
static const yytype_int16 yypact[] =
{
    1018,  3134,   433,   433,   433,   433,  -101,   207,  7224, -1335,
    5860,   689,   182,   109,  4532,   231,  3367,  3367, -1335, -1335,
   -1335,  1736, -1335, -1335, -1335,   347,   525,   316,   341,   143,
     366,   377,   569, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
     414,   473,   483,   495,   509,   524,   578,   623,   645,   657,
     682,   600, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,   703,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,   721,   725,
   -1335, -1335, -1335, -1335, -1335, -1335,   745, -1335,    79, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,   783,
     783, -1335,   718, -1335, -1335,   754, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,   486,
   -1335, -1335, -1335, -1335, -1335,   782, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335,   420, -1335,   796, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,   105, -1335,
   -1335,    27, -1335, -1335, -1335, -1335, -1335,   764, -1335,    37,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335,   608,   627, -1335,
   -1335, -1335,   847, -1335, -1335,   777,   672, -1335, -1335, -1335,
   -1335, -1335,   735,   756, -1335,   971,   716,   122,   124,   102,
     655, -1335,   775,   747,   765,   771, -1335, -1335, -1335,   313,
   -1335, -1335,  4532,   219, -1335, -1335, -1335,  5860, -1335, -1335,
    5860, -1335, -1335, -1335, -1335,   899, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,  3134, -1335,
    3134, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335,   279,   792,   773, -1335,
   -1335, -1335,  3600,   753, -1335, -1335, -1335, -1335,  3833,   753,
   -1335, -1335, -1335, -1335,  4066,   753, -1335, -1335,   753, -1335,
   -1335, -1335,  4299, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335,   912,   929,   948,   954,   963, -1335, -1335, -1335, -1335,
     770, -1335,   982,   219, -1335,  7224, -1335, -1335, -1335,   632,
   -1335,  6075,  3134, -1335,   968,   992,  3134,    69,   995,  7041,
    3134, -1335,   991,  7224,  7224,  1011,  3134, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,  3134,  7224,
      63, -1335,  3134,  3134,  7224,  1021, -1335,  7224,  1048,  1026,
   -1335,  7224,    96, -1335,   111,  3134,  3134,  3134,  1013,  1014,
   -1335,  6290,   968,   156,   915,  3134,   914, -1335, -1335,  3134,
   -1335, -1335,  7224,  3134,  3134,  3134,   745,    79,  3134,   939,
     940, -1335, -1335,   712, -1335,  3367,  3367,  3367,  3367,  3367,
    3367,  3367,  3367, -1335,  3367,  3367,  3367,  3367,  3367,  3367,
    3367,  3367,  4966,  3367,  3367,  3367,  3367,  3367,  3367,  3367,
    3367,  3367,  3367,  3367,   920,   935,   936,   938,  1969, -1335,
     219,  4532,  4532,  1070, -1335,  1070, -1335,  2202,  1070,   -11,
   -1335,  2435,  1036,   967,  7224,   767,   706,  1042,  1042,  1042,
    1042, -1335,  1042,  1042, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335,  1042, -1335, -1335,  1042, -1335, -1335,   516,   866,  1042,
    1042,  1042,  1042,  1042,  1042,  1042,  1042, -1335,  1042, -1335,
   -1335,   519,   785,  1042,  1042,  1042, -1335,  1042,  1042, -1335,
   -1335, -1335, -1335, -1335,  1042, -1335,  1042, -1335, -1335,  1042,
   -1335, -1335,   533, -1335, -1335,   563,   510,  1042,  1042,  1042,
    1042,  1042,  1042,  1042,  1042, -1335,  1042,   346,   548,   990,
     993, -1335, -1335, -1335,    78, -1335, -1335, -1335,   695, -1335,
   -1335,   320,  2668, -1335, -1335,   422, -1335,   377,   963,  1054,
    1055, -1335, -1335,  1056,  1059, -1335,   447,  2901, -1335,  1060,
   -1335,  1061, -1335, -1335, -1335,   489,   503,  1095,   703,  3134,
     518,   537,   915,   745,   915,  7224,    79,   915,   783,  3134,
    3134,   705,   709,   550,  3134,  3134, -1335, -1335,   715, -1335,
   -1335,  7224, -1335,   730, -1335,  6674,   811,   881,  1045,   956,
     884,   248, -1335, -1335, -1335,  3134,   973, -1335,   756, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335,  3367, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335,  4966,  3134,  5183,   477,   477,
     477,   477,  1022,  1091,  1096,  1030, -1335, -1335, -1335,  1078,
     124,   124,   861,   102,   102,   102,   102,   655,   655, -1335,
   -1335,  6674,  6674,  7224,  7224, -1335,   560, -1335, -1335,  3134,
   -1335, -1335,   731,   442,  3134,  3134,  3134, -1335,   734, -1335,
      51,  1084,   729,   998,  1116,  7224,   736,   781,   391,   108,
    7224, -1335, -1335,   512,  7398,    35,    57, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335,   767,   706,   142, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,   866,   331,
   -1335,   728, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
    1044,   887, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335,   365, -1335, -1335,  1111,   805,   581, -1335,
     997, -1335, -1335,  7224, -1335, -1335, -1335,   589, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335,   598, -1335, -1335, -1335, -1335,
    3134, -1335,   994, -1335, -1335,  1017, -1335,   943,  1119, -1335,
     999, -1335, -1335, -1335,   984,   986, -1335,   614,   618,  7224,
   -1335,   915,  1129, -1335,  6858,  1100,  1101,  1102, -1335,   403,
   -1335, -1335, -1335, -1335, -1335,  3134,  3134, -1335,  3134,  3134,
    3134,  1103,  1104,  1477, -1335,  1117, -1335,   790,  3134, -1335,
    1108,   621, -1335,  1057,   941, -1335, -1335, -1335, -1335, -1335,
    1063, -1335,  4966,   667,  4966,  1027, -1335, -1335, -1335,  1133,
   -1335, -1335,   529,  3134, -1335,   932,   937, -1335, -1335, -1335,
   -1335,  1041, -1335,   998, -1335, -1335, -1335, -1335,  7224,  1131,
   -1335, -1335, -1335, -1335,  1031,   998,  1001,  1002, -1335, -1335,
    1137,  1051,  7224,   -17,   942,   796,  7224,   988,   998,  1065,
    7224,  1065,   850,   791,   874, -1335, -1335,  1062,   285, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
    7398, -1335, -1335, -1335, -1335, -1335, -1335,   681, -1335,   224,
   -1335,   204, -1335,   303, -1335,   272, -1335,  7954, -1335,   944,
     260, -1335, -1335,   671, -1335, -1335,   886,   927,   890, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335,   216, -1335, -1335, -1335,   -30,
   -1335,   911, -1335,   950, -1335, -1335, -1335, -1335,   215, -1335,
     307, -1335,   893, -1335,  -120, -1335, -1335, -1335,   259, -1335,
     -76, -1335,   -36, -1335,   -92, -1335,   707, -1335,   896,   231,
     231, -1335, -1335, -1335,  1151, -1335, -1335, -1335, -1335,  3134,
    1186,   943,  1188,  1189,  3134,  3134,  4749,    21, -1335,  3134,
   -1335, -1335,   203, -1335, -1335,  1155,  1157,  1158,  6490, -1335,
   -1335, -1335, -1335,   626,   637, -1335,   977, -1335,  1196,  1196,
   -1335, -1335,   981, -1335,  3134, -1335, -1335,  1071,  1117, -1335,
     477, -1335, -1335,  3367,  1096,  1115,   898, -1335,  1159,  3367,
      28,    28,   784,   784, -1335, -1335,  1030,  4966, -1335, -1335,
   -1335, -1335,  3134,  3134,  1042,  1042,   915,   998,   860, -1335,
     998,   998,   829, -1335,   101, -1335, -1335, -1335,  1179, -1335,
    5414,    77,  1176,  1046,  1065,   998,  1183,  1181,  1183, -1335,
   -1335, -1335, -1335, -1335,  1008, -1335, -1335, -1335, -1335, -1335,
   -1335,    87,  1009, -1335,  1130,   796,   947,   949, -1335, -1335,
    5414,  6674,   772,   972, -1335, -1335,  6674,   972, -1335, -1335,
   -1335,  6674,  3134,  1139, -1335,  6674,  3134,  7399, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335,     3,  1109,  1109, -1335, -1335, -1335,   996,
    3134,   934,   757, -1335, -1335,  3134,  3134, -1335,  7427,   -51,
    3134,    94, -1335,   448, -1335,  7455, -1335,  3134, -1335, -1335,
     970, -1335,  6674,  3134,  7483,  3134,  7511, -1335,  3134,  7539,
    7567, -1335,  3134, -1335, -1335,  7595, -1335,  3134,  7623, -1335,
   -1335,  7651, -1335, -1335,  3134, -1335, -1335,  7679,  3134,  7707,
    1192,  1194,   455, -1335, -1335,  7224,  1106,  7224,  3134, -1335,
   -1335,  7224,  1113,   130, -1335, -1335,  1079,  1203, -1335, -1335,
   -1335, -1335,  1097, -1335,   743, -1335, -1335,  3134,  7224,   744,
   -1335,   746,  1208, -1335,   872,  1107, -1335,   102, -1335, -1335,
   -1335, -1335,   126,    45,  3367,  3367,   619,  1053, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335,    97, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335,   160, -1335,  1099,  1019,
     -17,  5860, -1335,  1024,   399,  1235,  1214,  1215,   481, -1335,
    5645,   668,   701, -1335,  1237, -1335,  6674, -1335, -1335, -1335,
   -1335,  1183, -1335,   998,   998, -1335,    -4,  1062, -1335, -1335,
     107,  1047,   123, -1335, -1335,    10,  1247, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335,   384,  7735, -1335,  7763, -1335,   847,
   -1335, -1335,   847, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335,  1138,  1138, -1335,   847,  3134, -1335, -1335,  1020,  7791,
     847,   847,   260, -1335, -1335, -1335, -1335, -1335,   847, -1335,
   -1335, -1335,   265, -1335, -1335, -1335,  7819,   847,   260,   260,
   -1335,  1025, -1335,   847, -1335,   847, -1335,   260,   847, -1335,
   -1335,   260,   847,   260,   260, -1335,   260,   847, -1335,   260,
     260, -1335,   847, -1335,   847, -1335,   231,   231,   689, -1335,
   -1335, -1335, -1335, -1335,  3134, -1335, -1335,  1121,  3134,  7224,
    3134,  3134, -1335,  6674,  6674,  1126, -1335,   915,  1196, -1335,
   -1335,  1196, -1335, -1335,   998, -1335, -1335, -1335, -1335,  3367,
    3367,   102,   141, -1335, -1335,  3134, -1335, -1335, -1335,   748,
    6674,   811,  5414, -1335, -1335,  5860,  5860,   115,  1062,  5414,
   -1335,  1099, -1335,  5645,  5645,  5645,  5645, -1335,  1242,   131,
   -1335,  1065, -1335, -1335,   264,  1062,  1062, -1335, -1335,  6674,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,  6674,  3134,
    7847, -1335, -1335, -1335,   847,  7875, -1335, -1335, -1335, -1335,
    6674,  3134,  7903, -1335,  7931, -1335,  1474,   751,   758,   231,
   -1335,  6674, -1335,  1162, -1335, -1335, -1335, -1335,  6674,  1279,
   -1335,   750, -1335,   102,   102,  3367, -1335, -1335, -1335, -1335,
     481, -1335, -1335,  1252, -1335, -1335,  1255, -1335,   811, -1335,
   -1335, -1335, -1335,  3134, -1335, -1335,  1183,   755,   759,  1062,
    1090,   760, -1335, -1335,   726,   726,   140, -1335,   847, -1335,
   -1335, -1335,   847, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335,  1271,  1168,  3134, -1335,  3134, -1335,   102, -1335,  1099,
   -1335,   408, -1335,   570,  1187, -1335, -1335,    28, -1335,  1062,
   -1335,   107,   107, -1335, -1335, -1335,   405,   410,   436,   458,
   -1335,  3134, -1335, -1335,   811,  1258, -1335, -1335,  1282, -1335,
    1098, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335,   354, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335,   132, -1335, -1335,  1300, -1335,  -452,  -840, -1335,
   -1335, -1335, -1335,   348, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335,   116, -1335, -1335, -1335, -1335, -1335, -1335, -1335,   128,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1127,  -157, -1335, -1335,
   -1335, -1335, -1335, -1123, -1335, -1335, -1335,  1023,  1028,  1032,
    -270,  -233,  -548, -1335,  1016,  1029,  1033, -1335,  1034,  1035,
    1007, -1335,   843,  1006,  1010,    49,  1037,  -287,  -265,  -451,
    -261,  -248,  -249,  -203,  -277,  -238,  -237,  -236, -1335, -1335,
    -202,  -172, -1335,  -918, -1335, -1335,  -168, -1335,  -111,  -266,
    -844, -1335,    29,   492,  -634,   342,   -21,  -220, -1335, -1335,
   -1335,  1134, -1335,   714, -1335,   298,  1136, -1335,   717, -1335,
     885,   484,   277, -1335, -1335, -1335, -1335,  1245,   719, -1335,
   -1335, -1335,   309, -1335, -1335,   894,   895, -1335,   258, -1335,
     875,   554,  -458,   549,   551, -1335, -1335, -1335,  1015, -1335,
   -1335, -1335, -1335,  1135, -1335, -1335, -1335,    72,   553, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335,    26, -1335, -1335, -1335,
    -228, -1335,  -442, -1335,  -461,  -383, -1335,  -428, -1335, -1335,
    -427,   851, -1335,   134, -1335,   -63, -1335,  -577, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335,  1219,   930, -1335, -1335, -1335, -1335, -1335, -1335,
     673, -1335,  -595,  -611, -1335,   542,   337,  -545, -1335, -1335,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335,  1003, -1335,  1038,
   -1335,   989,   952,   559,  -730,   737, -1335, -1335,   521,   520,
     317,   732, -1335, -1335,   532, -1335, -1335, -1335, -1335,   -73,
   -1310, -1335,   -77,  -549, -1335, -1335, -1335, -1335,  -639, -1072,
     137,   332, -1335, -1335, -1335, -1335, -1335, -1335, -1335, -1335,
   -1335, -1335,   798, -1335,   336, -1335,  -920,  -480,  -371,  -112,
   -1334, -1231,   315, -1335,   117, -1335, -1335, -1335, -1055,   -18,
   -1335, -1335, -1335, -1335, -1335, -1335, -1335,     6,     7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -538
static const yytype_int16 yytable[] =
{
     379,   495,   788,  1118,  1261,   919,   921,  1128,   498,  1277,
     590,   679,   847,  1336,   359,   360,  1483,   360,   693,   694,
     695,   696,   590,   613,   721,   567,   617,   815,   529,   817,
     678,   622,   820,   531,   624,   572,   366,   532,   627,   544,
     867,   868,   869,   545,   680,   681,   574,   568,   499,   533,
     500,   569,   300,   306,   311,   547,   546,   739,   740,   741,
     613,   742,   743,   570,   546,   530,   549,   550,   551,  1354,
     744,  1111,  1374,   745,   549,   550,   551,  1471,   749,   750,
     751,   752,   753,   754,   755,   756,   373,   757,   417,   437,
     876,   877,   762,   763,   764,   534,   765,   766,   618,   440,
     841,   548,   552,   767,   737,   768,  1105,  1455,   769,   571,
     552,  1327,   596,  1153,   480,  1157,   772,   773,   774,   775,
     776,   777,   778,   779,     8,   780,  1355,   845,   846,  1326,
     536,   628,   553,  1492,   597,   476,   554,  1548,   480,  1429,
     553,   713,   313,  1296,   573,  1188,   628,  1449,  1273,  1485,
    1222,   845,   846,   480,  1207,  1195,   841,   841,   473,   474,
    1204,   896,  1206,  1095,  1209,   641,  1210,   435,  1215,   641,
    1218,  1036,  1221,   845,   846,  1099,  1227,  1208,  1229,  1482,
     907,   845,   846,   896,   790,   389,   538,  1472,  1114,  1450,
     845,   846,   559,   555,   390,  1223,  1321,   477,  1216,   642,
     576,   555,  1327,  1458,  1243,  1224,   314,   318,   916,   791,
     436,   714,   715,   940,   679,   647,   679,   371,   917,   649,
     716,  1217,  1486,   651,   652,   653,  1247,   891,   654,  1316,
     920,   922,   923,   678,   360,   678,  1043,   360,  1219,  1274,
    1275,    81,   818,  1189,   629,   899,  1248,   680,   681,   680,
     681,   491,  1317,   494,   438,  1614,   496,   492,   831,   630,
     598,  1375,  1430,  1376,   441,  1220,   481,  1190,   391,   892,
    1328,   529,  1456,   418,   842,  1489,   531,   711,   567,   908,
     532,   711,   619,   475,  1619,   909,   478,   479,   572,   841,
     481,  1332,   533,   924,   490,  1106,  1107,   544,  1490,   574,
     568,   545,  1535,  1345,   569,   481,  1554,  1347,   530,  1589,
     845,   846,   370,   547,   546,  1604,   570,   546,  1356,  1357,
    1358,   501,  1493,  1494,   549,   550,   551,   549,   550,   551,
     842,   842,   842,   842,   502,  1540,  1379,   374,   534,  1445,
    1446,  1447,   913,  1380,  1369,   925,   926,    15,   300,   927,
    1549,  1550,  1551,  1552,   306,   444,  1386,   311,   387,   548,
     552,   591,   571,   552,   488,   595,   380,  1288,   928,   606,
    1291,  1292,   795,   536,  1129,   615,  1177,  1130,  1131,  1132,
    1133,   584,   360,   388,  1273,  1322,  1531,   616,   360,   381,
     553,   620,   621,   553,   554,   605,   360,   573,   382,   812,
     360,   360,   937,   938,   631,   632,   633,  1484,   393,   822,
     823,   679,  1560,   679,  1049,   359,   360,     8,  1465,   394,
     359,   360,  1025,   359,   360,  1242,  1050,   359,   360,   538,
     678,  1266,   678,   397,   939,   854,  1112,  1253,   360,  1051,
    1117,   381,  1230,  1231,   680,   681,   680,   681,   650,   360,
     559,   555,  1624,   576,   555,   845,   846,   444,  1025,  1627,
     845,   846,   431,   842,    22,    23,    24,   706,  1134,  1559,
    1297,   382,   383,   852,   798,  1274,  1275,  1318,  1144,   689,
    1196,    11,   444,  1630,    12,    13,   845,   846,  1418,   853,
    1145,   841,   398,  1469,   887,   888,   889,  1138,  1139,   803,
    1146,  1287,   399,   841,  1500,   845,   846,  1633,   845,   846,
     605,   360,  1197,  1186,   400,  1096,  1187,  1198,  1199,  1140,
    1141,  1625,  1142,  1296,   444,  1620,  1505,  1628,   401,   904,
    1337,   845,   846,  1211,    81,  1346,   905,   427,   444,  1508,
    1348,   808,  1512,   402,  1351,   383,  1154,  1545,  1546,  1509,
    1514,  1090,  1631,   444,  1516,   809,  1212,   428,   429,  1213,
    1214,  1510,  1511,  1556,   444,  1562,  1563,   385,  1155,  1156,
     813,   797,   444,  1419,  1420,  1634,   386,  1148,   722,   748,
     723,  1201,   906,   731,   732,   444,   805,  1149,  1150,   814,
    1028,  1392,  1618,  1480,  1481,   444,   841,   403,  1421,  1151,
    1152,   841,   826,  1202,  1203,   444,   841,   941,  1021,   942,
     841,   395,   881,   827,   828,  1267,   943,   944,   781,   287,
     396,  1272,   945,   288,   444,  1022,   679,   782,  1055,  1056,
    1057,   359,   360,   444,   523,   946,   947,   724,   854,  1597,
     537,  1026,   404,  1285,  1286,   678,   557,   359,   360,   444,
    1027,   408,   360,   444,   575,   861,   444,   841,  1495,   680,
     681,   444,   885,  1091,   405,   842,  1040,   444,  1496,  1497,
    1041,   886,   444,  1070,  1470,   585,   406,   842,  1255,  1621,
    1498,  1499,   689,   725,   689,   863,   864,   865,   882,  1256,
    1383,   726,   727,   728,   729,   730,   746,  1380,  1491,   758,
    1473,   407,  -361,  -361,   747,  1479,  1474,   747,   360,   360,
     360,   360,   409,   770,  1532,   660,   661,   662,   663,   664,
     665,   747,   667,   668,   669,   670,   671,   672,   673,   674,
     793,   605,   360,  1475,   759,  1372,   911,   360,   794,  1476,
     444,   915,   760,   771,   444,   733,  1388,  1389,   824,   912,
     829,   747,   825,   913,   414,  1397,   922,   923,   830,  1401,
     425,  1403,  1404,  1075,  1406,   832,   883,  1409,  1410,   883,
     842,   841,   412,   833,   884,   842,   413,   890,  1434,  1438,
     842,  1438,  1076,   832,   842,  1438,  1435,  1439,  1077,  1440,
    1594,  1537,   421,  1586,  1594,  1599,   426,  1575,  1595,  1576,
     360,   430,  1596,  1600,  1578,   446,  1579,   447,   448,  1234,
     449,   450,   367,   368,  1239,  1240,  1451,  1452,   924,  1246,
     482,   483,  1526,  1527,  1053,  1054,   439,  1539,  1338,  1339,
    1340,   842,  1445,  1446,  1447,   722,   360,   723,   433,  1517,
    1518,   360,  1529,   442,   854,  1555,   299,   305,   308,  1538,
     449,   450,  1423,   722,  1425,   723,   784,   785,  1427,  1063,
     360,   845,   846,  1078,  1079,  1080,  1081,   443,  1082,  1083,
     925,   926,  1283,  1284,   927,  1437,   580,   581,  1566,   689,
    1121,   689,   444,  1122,  1123,  1124,  1125,  1567,   841,   841,
    1601,   455,  1602,   928,   724,   735,   736,   473,   474,  1571,
     894,   895,   902,   903,   359,   360,  1588,   900,   901,   445,
    1582,   472,   724,   456,  1592,   841,   485,  1584,  1104,   360,
     731,   732,   605,   360,   859,   486,   605,   360,  1019,  1020,
    1065,  1066,  1605,   484,   722,   748,   723,  1119,  1120,   487,
     725,   497,  1581,   288,   841,   842,   504,  1135,   726,   727,
     728,   729,   730,   841,   381,   722,   748,   723,   725,   731,
     732,  1126,  1127,  1136,  1137,   841,   726,   727,   728,   729,
     730,   385,  1626,  1629,  1632,  1635,   841,  -361,  -361,   503,
    1637,   980,   981,   841,   457,   458,   459,   460,   461,   462,
     389,  1533,  1534,   724,  1225,  1226,   395,   463,  1269,  1270,
    1278,  1279,   733,  1289,  1290,   431,  1544,  1293,  1294,   759,
     464,  1181,  1366,   583,   724,  1442,  1443,   760,  1426,   592,
     733,     1,     2,     3,     4,     5,     6,   690,   691,   697,
     698,   376,   377,   699,   700,   594,  -211,  1436,   601,   725,
     879,   879,   607,   360,   707,   708,  1523,   726,   727,   728,
     729,   730,  1622,  1623,   614,   360,   623,   625,   731,   732,
     725,   626,   842,   842,   634,   635,  -211,   645,   726,   727,
     728,   729,   730,   648,  -211,  -211,   655,  1587,   656,   701,
     465,   466,   467,   468,   469,   470,  1478,   702,   703,   842,
     704,   709,   719,   720,   689,   737,   787,   799,   800,   801,
     789,   733,   802,   806,   807,   810,   848,   849,   850,   851,
     858,   759,   722,   870,   723,   872,   875,   360,   842,   760,
     501,   874,   733,   893,   896,   898,  1029,   842,  1023,  1034,
    1617,  1349,  1030,  1035,  1036,  1352,  1315,  1032,   641,   842,
    1039,   471,  1046,  1047,  1048,  1058,  1059,   360,   360,  1616,
     842,  1069,  1064,   360,  1072,  1071,  1088,   842,   360,  1364,
    1073,  1087,   360,  1092,  1370,  1371,  1315,  1094,  1093,  1378,
    1097,   724,  1102,  1098,  1100,  1101,  1387,  1103,  1110,  1113,
    1115,  1181,  1393,  1180,  1395,  1192,  1176,  1398,    81,  1185,
    1205,  1402,  1194,  1228,  1233,  1235,  1407,  1237,  1249,  1238,
    1250,  1251,  1257,  1412,  1520,  1258,  1262,  1414,  1522,   360,
    1524,  1525,  1268,  1264,  1300,  1319,    23,   725,  1323,  1320,
    1324,  1325,  1330,  1342,  1331,   726,   727,   728,   729,   730,
    1333,  1334,  1360,  1365,  1431,  1536,  1390,  1416,  1363,  1417,
    1424,   359,   360,   359,   360,  1428,  1432,   359,   360,  1433,
    1441,  1460,  1444,  1454,  1466,  1462,  1467,  1468,  -536,  1469,
    1488,  1503,  1506,  1553,   359,   360,  -211,  1515,   759,  -211,
    -211,  -211,  -211,  1521,  -211,  -211,   760,  -211,  1528,   733,
    -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,  -211,  1585,
    1591,  -211,  -211,  -211,  1583,  1590,  1598,  -211,  -211,  1610,
    1611,  1638,  1327,  -537,  1639,  1147,   317,  1362,   360,  1384,
    1367,  1179,  1547,   562,   563,   539,   525,   360,   564,  -211,
    -211,   526,  -211,   360,  1044,  1459,   527,  1464,   540,  1236,
     451,  -211,   452,   541,  -211,  -211,  1315,   816,   657,   560,
     561,  1263,  1068,   819,  1504,   424,  1245,   821,   565,   658,
     692,   659,   718,   961,   489,  1519,   962,   963,   964,   965,
     432,   966,   640,  1612,   967,  1613,  1422,   968,   969,   970,
     971,   972,   973,   974,   975,   976,  1045,   880,   977,   978,
     979,  1350,   610,   639,   962,   963,   964,   965,  1042,   966,
     599,  1636,   967,  1074,  1086,   968,   969,   970,   971,   972,
     973,   974,   975,   976,  1282,  1085,   977,   978,   979,   862,
    1558,  1565,   860,  1453,  1271,  1281,   811,  1463,   314,  1299,
    1530,   980,   981,     0,     0,   600,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   359,   360,     0,     0,     0,
     360,   360,     0,     0,     0,     0,   314,     0,     0,   980,
     981,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   360,     0,   360,
       0,     0,   360,   360,     0,     0,   360,     0,  1568,     0,
     360,   360,   360,   360,     0,     0,     0,     0,  1315,     0,
    1572,  1541,  1542,     0,     0,  1315,   360,     0,     0,  1315,
    1315,  1315,  1315,     0,     0,   360,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   360,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   360,     0,
       0,     0,  1593,     0,     0,   360,   319,   320,   321,   322,
     323,   324,   325,   326,    33,    34,    35,    36,    37,    38,
      39,   327,   328,   329,   330,   331,   332,   333,   334,   335,
     336,   337,   338,    52,    53,    54,    55,    56,     0,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,   339,    70,     0,     0,     0,    71,    72,    73,    74,
      75,    76,    77,     0,     0,     0,     0,     0,   340,   341,
      80,     0,     0,     0,    82,     0,     0,     0,     0,    85,
     342,    87,   343,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,   344,   345,   101,   346,   103,     0,   104,
     347,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   348,   120,   121,   122,   123,   124,
     349,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,     0,   138,   139,   140,     0,   350,   142,
     351,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   352,   169,   170,   353,   172,
     173,   174,   175,     0,   176,   354,   178,   355,   180,   181,
     182,   183,   184,   185,   186,   356,   357,   189,   190,   962,
     963,   964,   965,     0,   966,     0,     0,   967,     0,     0,
     968,   969,   970,   971,   972,   973,   974,   975,   976,     0,
       0,   977,   978,   979,     0,     8,     0,     9,     0,     0,
       0,     0,     0,     0,     0,     0,    10,     0,     0,     0,
       0,     0,    11,     0,     0,    12,    13,     0,    14,     0,
      15,     0,    16,    17,    18,     0,    19,    20,    21,   378,
       0,   314,     0,     0,   980,   981,     0,     0,     0,  1060,
       0,  1061,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,     0,     0,     0,    71,    72,    73,    74,    75,
      76,    77,     0,     0,     0,     0,     0,    78,    79,    80,
       0,     0,    81,    82,    83,    84,     0,     0,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,     0,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,     0,   138,   139,   140,     0,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,     0,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,     8,     0,
       9,     0,     0,     0,     0,     0,     0,     0,     0,    10,
       0,     0,     0,     0,     0,    11,     0,     0,    12,    13,
       0,    14,     0,    15,     0,    16,    17,    18,     0,    19,
      20,    21,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   705,     0,     0,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
       0,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,     0,     0,     0,    71,    72,
      73,    74,    75,    76,    77,     0,     0,     0,     0,     0,
      78,    79,    80,     0,     0,    81,    82,    83,    84,     0,
       0,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
       0,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,     0,   138,   139,   140,     0,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,     0,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,     8,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,    10,     0,     0,     0,     0,     0,    11,     0,
       0,    12,    13,     0,    14,     0,    15,     0,    16,    17,
      18,     0,    19,    20,    21,   710,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,     0,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,     0,     0,
       0,    71,    72,    73,    74,    75,    76,    77,     0,     0,
       0,     0,     0,    78,    79,    80,     0,     0,    81,    82,
      83,    84,     0,     0,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,     0,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,     0,   138,
     139,   140,     0,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,     0,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,     8,     0,     9,     0,     0,     0,
       0,     0,     0,     0,     0,    10,     0,     0,     0,     0,
       0,    11,     0,     0,    12,    13,     0,    14,     0,    15,
       0,    16,    17,    18,     0,    19,    20,    21,   717,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,     0,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,     0,     0,     0,    71,    72,    73,    74,    75,    76,
      77,     0,     0,     0,     0,     0,    78,    79,    80,     0,
       0,    81,    82,    83,    84,     0,     0,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,     0,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,     0,   138,   139,   140,     0,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,     0,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,     8,     0,     9,
       0,     0,     0,     0,     0,     0,     0,     0,    10,     0,
       0,     0,     0,     0,    11,     0,     0,    12,    13,     0,
      14,     0,    15,     0,    16,    17,    18,     0,    19,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     796,     0,     0,     0,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,     0,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,     0,     0,     0,    71,    72,    73,
      74,    75,    76,    77,     0,     0,     0,     0,     0,    78,
      79,    80,     0,     0,    81,    82,    83,    84,     0,     0,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,     0,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,     0,   138,   139,   140,     0,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,     0,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
       8,     0,     9,     0,     0,     0,     0,     0,     0,     0,
       0,    10,     0,     0,     0,     0,     0,    11,     0,     0,
      12,    13,     0,    14,     0,    15,     0,    16,    17,    18,
       0,    19,    20,    21,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   804,     0,     0,     0,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,     0,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,     0,     0,     0,
      71,    72,    73,    74,    75,    76,    77,     0,     0,     0,
       0,     0,    78,    79,    80,     0,     0,    81,    82,    83,
      84,     0,     0,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,     0,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,     0,   138,   139,
     140,     0,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,     0,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,     8,     0,     9,     0,     0,     0,     0,
       0,     0,     0,     0,    10,     0,     0,     0,     0,     0,
      11,     0,     0,    12,    13,     0,    14,     0,    15,     0,
      16,    17,    18,     0,    19,    20,    21,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
       0,     0,     0,    71,    72,    73,    74,    75,    76,    77,
       0,     0,     0,     0,     0,    78,    79,    80,     0,     0,
      81,    82,    83,    84,     0,     0,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,     0,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
       0,   138,   139,   140,     0,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
       0,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,     8,     0,     9,     0,
       0,     0,     0,     0,     0,     0,     0,    10,     0,     0,
       0,     0,     0,    11,     0,     0,    12,    13,     0,    14,
       0,    15,     0,    16,    17,    18,     0,    19,    20,    21,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,     0,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,   339,    70,     0,     0,     0,    71,    72,    73,    74,
      75,    76,    77,     0,     0,     0,     0,     0,    78,    79,
      80,     0,     0,    81,    82,    83,    84,     0,     0,    85,
     342,    87,   343,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,   344,   345,   101,   346,   103,     0,   104,
     347,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,     0,   138,   139,   140,     0,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   352,   169,   170,   353,   172,
     173,   174,   175,     0,   176,   354,   178,   355,   180,   181,
     182,   183,   184,   185,   186,   356,   357,   189,   190,     8,
       0,     9,     0,     0,     0,     0,     0,     0,     0,     0,
      10,     0,     0,     0,     0,     0,    11,     0,     0,    12,
      13,     0,    14,     0,    15,     0,    16,    17,    18,     0,
      19,    20,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,     0,     0,     0,    71,
      72,    73,    74,    75,    76,    77,     0,     0,     0,     0,
       0,    78,    79,    80,     0,     0,    81,    82,    83,    84,
       0,     0,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,     0,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     505,   133,   134,   135,   136,   137,     0,   138,   506,   140,
       0,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,     0,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,     8,     0,     9,     0,     0,     0,     0,     0,
       0,     0,     0,    10,     0,     0,     0,     0,     0,    11,
       0,     0,    12,    13,     0,    14,     0,    15,     0,    16,
      17,    18,     0,    19,    20,    21,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,     0,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,     0,
       0,     0,    71,    72,    73,    74,    75,    76,    77,     0,
       0,     0,     0,     0,    78,    79,    80,     0,     0,    81,
      82,    83,    84,     0,     0,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,     0,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   528,   133,   134,   135,   136,   137,     0,
     138,   506,   140,     0,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,     0,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,     8,     0,     9,     0,     0,
       0,     0,     0,     0,     0,     0,    10,     0,     0,     0,
       0,     0,    11,     0,     0,    12,    13,     0,    14,     0,
      15,     0,    16,    17,    18,     0,    19,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,     0,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,     0,     0,     0,    71,    72,    73,    74,    75,
      76,    77,     0,     0,     0,     0,     0,    78,    79,    80,
       0,     0,    81,    82,    83,    84,     0,     0,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,     0,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   542,   133,   134,   135,
     136,   137,     0,   138,   506,   140,     0,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,     0,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,     8,     0,
       9,     0,     0,     0,     0,     0,     0,     0,     0,    10,
       0,     0,     0,     0,     0,    11,     0,     0,    12,    13,
       0,    14,     0,    15,     0,    16,    17,    18,     0,    19,
      20,    21,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
       0,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,     0,     0,     0,    71,    72,
      73,    74,    75,    76,    77,     0,     0,     0,     0,     0,
      78,    79,    80,     0,     0,    81,    82,    83,    84,     0,
       0,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
       0,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   566,
     133,   134,   135,   136,   137,     0,   138,   506,   140,     0,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,     0,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,     8,     0,     9,     0,     0,     0,     0,     0,     0,
       0,     0,    10,     0,     0,     0,     0,     0,    11,     0,
       0,    12,    13,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    19,    20,    21,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,     0,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,   339,    70,     0,     0,
       0,    71,    72,    73,    74,    75,    76,    77,     0,     0,
       0,     0,     0,    78,    79,    80,     0,     0,    81,    82,
      83,    84,     0,     0,    85,   342,    87,   343,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,   344,   345,
     101,   346,   103,     0,   104,   347,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   348,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,     0,   138,
     139,   140,     0,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     352,   169,   170,   353,   172,   173,   174,   175,  1241,   176,
     354,   178,   355,   180,   181,   182,   183,   184,   185,   186,
     356,   357,   189,   190,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   834,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   361,   362,
      27,    28,   363,    30,    31,   364,   835,    34,    35,    36,
      37,    38,    39,   327,   328,   329,   330,   331,   332,   333,
     334,   335,   336,   337,   338,    52,    53,    54,    55,    56,
       0,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,   339,    70,     0,     0,     0,    71,    72,
      73,    74,    75,    76,    77,     0,     0,     0,     0,     0,
     340,   341,    80,     0,     0,     0,    82,     0,     0,     0,
       0,    85,   342,    87,   343,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,   344,   345,   101,   346,   103,
       0,   104,   347,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   348,   120,   121,   122,
     123,   124,   349,   836,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,     0,   138,   139,   140,     0,
     365,   142,   837,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   352,   169,   170,
     353,   172,   173,   174,   175,     8,   176,   354,   178,   355,
     180,   181,   182,   183,   184,   185,   186,   356,   357,   189,
     190,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    19,     0,   675,     0,
       0,     0,     0,     0,     0,     0,     0,   676,     0,     0,
     677,     0,    22,    23,    24,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    34,    35,    36,    37,    38,    39,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     337,   338,    52,    53,    54,    55,    56,     0,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
     339,    70,     0,     0,     0,    71,    72,    73,    74,    75,
      76,    77,     0,     0,     0,     0,     0,   340,   341,    80,
       0,     0,    81,    82,     0,     0,     0,     0,    85,   342,
      87,   343,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,   344,   345,   101,     0,   103,     0,   104,     0,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   348,   120,   121,   122,   123,   124,   349,
       0,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,     0,   138,   139,   140,     0,     0,   142,     0,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   352,   169,   170,   353,   172,   173,
     174,   175,     8,   176,   354,   178,   355,   180,   181,   182,
     183,   184,   185,   186,   356,   357,   189,   190,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    19,     0,   675,     0,     0,     0,     0,
       0,     0,     0,     0,   676,     0,     0,     0,     0,    22,
      23,    24,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    34,    35,    36,    37,    38,    39,   327,   328,   329,
     330,   331,   332,   333,   334,   335,   336,   337,   338,    52,
      53,    54,    55,    56,     0,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,   339,    70,     0,
       0,     0,    71,    72,    73,    74,    75,    76,    77,     0,
       0,     0,     0,     0,   340,   341,    80,     0,     0,    81,
      82,     0,     0,     0,     0,    85,   342,    87,   343,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,   344,
     345,   101,     0,   103,     0,   104,     0,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     348,   120,   121,   122,   123,   124,   349,     0,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,     0,
     138,   139,   140,     0,     0,   142,     0,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   352,   169,   170,   353,   172,   173,   174,   175,     0,
     176,   354,   178,   355,   180,   181,   182,   183,   184,   185,
     186,   356,   357,   189,   190,     9,     0,     0,     0,     0,
       0,     0,     0,     0,  1301,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1302,     0,     0,     0,
       0,     0,  1303,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,  1304,   362,    27,    28,   363,    30,    31,
     364,    33,    34,    35,    36,    37,    38,    39,  1305,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
      52,    53,    54,    55,    56,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,   339,    70,
       0,     0,     0,    71,    72,    73,    74,    75,    76,    77,
       0,     0,     0,     0,     0,   340,   341,    80,     0,     0,
       0,    82,    83,    84,     0,     0,    85,   342,    87,   343,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
     344,   345,   101,   346,   103,     0,   104,   347,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   348,   120,   121,   122,   123,   124,   349,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
       0,   138,   139,   140,     0,   365,   142,   351,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   352,   169,   170,   353,   172,   173,   174,   175,
       0,   176,   354,   178,   355,   180,   181,  1306,  1307,   184,
     185,   186,   356,   357,   189,   190,     9,     0,     0,     0,
       0,     0,     0,     0,     0,  1301,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,  1304,   362,    27,    28,   363,    30,
      31,   364,    33,    34,    35,    36,    37,    38,    39,  1305,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,    52,    53,    54,    55,    56,     0,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,   339,
      70,     0,     0,     0,    71,    72,    73,    74,    75,    76,
      77,     0,     0,     0,     0,     0,   340,   341,    80,     0,
       0,     0,    82,    83,    84,     0,     0,    85,   342,    87,
     343,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,   344,   345,   101,   346,   103,     0,   104,   347,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   348,   120,   121,   122,   123,   124,   349,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,     0,   138,   139,   140,     0,   365,   142,   351,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   352,   169,   170,   353,   172,   173,   174,
     175,     9,   176,   354,   178,   355,   180,   181,   182,   183,
     184,   185,   186,   356,   357,   189,   190,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   361,
     362,    27,    28,   363,    30,    31,   364,    33,    34,    35,
      36,    37,    38,    39,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,    52,    53,    54,    55,
      56,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,   339,    70,     0,     0,     0,    71,
      72,    73,    74,    75,    76,    77,     0,     0,     0,     0,
       0,   340,   341,    80,     0,     0,     0,    82,    83,    84,
       0,     0,    85,   342,    87,   343,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,   344,   345,   101,   346,
     103,     0,   104,   347,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   348,   120,   121,
     122,   123,   124,   349,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,     0,   138,   139,   140,
       0,   365,   142,   351,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   352,   169,
     170,   353,   172,   173,   174,   175,   586,   176,   354,   178,
     355,   180,   181,   182,   183,   184,   185,   186,   356,   357,
     189,   190,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   587,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   319,   320,   321,   322,   323,   324,
     325,   326,    33,    34,    35,    36,    37,    38,    39,   327,
     328,   329,   330,   331,   332,   333,   334,   335,   336,   337,
     338,    52,    53,    54,    55,    56,     0,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,   339,
      70,     0,     0,     0,    71,    72,    73,    74,    75,    76,
      77,     0,     0,     0,     0,     0,   340,   341,    80,     0,
       0,     0,    82,     0,     0,     0,     0,    85,   342,    87,
     343,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,   344,   345,   101,   346,   103,     0,   104,   347,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   348,   120,   121,   122,   123,   124,   349,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,     0,   138,   139,   140,     0,   350,   142,   351,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   352,   169,   170,   353,   172,   173,   174,
     175,   636,   176,   354,   178,   355,   180,   181,   182,   183,
     184,   185,   186,   356,   357,   189,   190,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   637,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   319,
     320,   321,   322,   323,   324,   325,   326,    33,    34,    35,
      36,    37,    38,    39,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,    52,    53,    54,    55,
      56,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,   339,    70,     0,     0,     0,    71,
      72,    73,    74,    75,    76,    77,     0,     0,     0,     0,
       0,   340,   341,    80,     0,     0,     0,    82,     0,     0,
       0,     0,    85,   342,    87,   343,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,   344,   345,   101,   346,
     103,     0,   104,   347,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   348,   120,   121,
     122,   123,   124,   349,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,     0,   138,   139,   140,
       0,   350,   142,   351,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   352,   169,
     170,   353,   172,   173,   174,   175,     0,   176,   354,   178,
     355,   180,   181,   182,   183,   184,   185,   186,   356,   357,
     189,   190,   834,  1252,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   361,
     362,    27,    28,   363,    30,    31,   364,   835,    34,    35,
      36,    37,    38,    39,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,   337,   338,    52,    53,    54,    55,
      56,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,   339,    70,     0,     0,     0,    71,
      72,    73,    74,    75,    76,    77,     0,     0,     0,     0,
       0,   340,   341,    80,     0,     0,     0,    82,     0,     0,
       0,     0,    85,   342,    87,   343,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,   344,   345,   101,   346,
     103,     0,   104,   347,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   348,   120,   121,
     122,   123,   124,   349,   836,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,     0,   138,   139,   140,
       0,   365,   142,   837,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   352,   169,
     170,   353,   172,   173,   174,   175,   834,   176,   354,   178,
     355,   180,   181,   182,   183,   184,   185,   186,   356,   357,
     189,   190,     0,   361,   362,    27,    28,   363,    30,    31,
     364,   835,    34,    35,    36,    37,    38,    39,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
      52,    53,    54,    55,    56,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,   339,    70,
       0,     0,     0,    71,    72,    73,    74,    75,    76,    77,
       0,     0,     0,     0,     0,   340,   341,    80,     0,     0,
       0,    82,     0,     0,     0,     0,    85,   342,    87,   343,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
     344,   345,   101,   346,   103,     0,   104,   347,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   348,   120,   121,   122,   123,   124,   349,   836,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
       0,   138,   139,   140,     0,   365,   142,   837,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   352,   169,   170,   353,   172,   173,   174,   175,
     834,   176,   354,   178,   355,   180,   181,   182,   183,   184,
     185,   186,   356,   357,   189,   190,     0,   361,   362,    27,
      28,   363,    30,    31,   364,    33,    34,    35,    36,    37,
      38,    39,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,    52,    53,    54,    55,    56,     0,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,   339,    70,     0,     0,     0,    71,    72,    73,
      74,    75,    76,    77,     0,     0,     0,     0,     0,   340,
     341,    80,     0,     0,     0,    82,     0,     0,     0,     0,
      85,   342,    87,   343,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,   344,   345,   101,   346,   103,     0,
     104,   347,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   348,   120,   121,   122,   123,
     124,   349,   836,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,     0,   138,   139,   140,     0,   365,
     142,   837,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   352,   169,   170,   353,
     172,   173,   174,   175,   602,   176,   354,   178,   355,   180,
     181,   182,   183,   184,   185,   186,   356,   357,   189,   190,
     319,   320,   321,   322,   323,   324,   325,   326,    33,    34,
      35,    36,    37,    38,    39,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,    52,    53,    54,
      55,    56,     0,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,   339,    70,     0,     0,     0,
      71,    72,    73,    74,    75,    76,    77,     0,     0,     0,
       0,     0,   340,   341,    80,     0,     0,   603,    82,     0,
       0,     0,     0,    85,   342,    87,   343,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,   344,   345,   101,
     346,   103,     0,   104,   347,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   348,   120,
     121,   122,   123,   124,   349,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,     0,   138,   139,
     140,     0,   350,   142,   351,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   352,
     169,   170,   353,   172,   173,   174,   175,     0,   176,   354,
     178,   355,   180,   181,   182,   183,   184,   185,   186,   356,
     357,   189,   190,   319,   320,   321,   322,   323,   324,   325,
     326,    33,    34,    35,    36,    37,    38,    39,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
      52,    53,    54,    55,    56,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,   339,    70,
       0,     0,     0,    71,    72,    73,    74,    75,    76,    77,
       0,     0,     0,     0,     0,   340,   341,    80,     0,     0,
       0,    82,     0,     0,     0,     0,    85,   342,    87,   343,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
     344,   345,   101,   346,   103,     0,   104,   347,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   348,   120,   121,   122,   123,   124,   349,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
       0,   138,   139,   140,     0,   350,   142,   351,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   352,   169,   170,   353,   172,   173,   174,   175,
       0,   176,   354,   178,   355,   180,   181,   182,   183,   184,
     185,   186,   356,   357,   189,   190,    34,    35,    36,    37,
      38,    39,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,    52,    53,    54,    55,    56,     0,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,   339,    70,     0,     0,     0,    71,    72,    73,
      74,    75,    76,    77,     0,     0,     0,     0,     0,   340,
     341,    80,     0,     0,     0,    82,     0,     0,     0,     0,
      85,   342,    87,   343,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,   344,   345,   101,     0,   103,     0,
     104,     0,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   348,   120,   121,   122,   123,
     124,   349,     0,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,     0,   138,   139,   140,     0,     0,
     142,     0,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   352,   169,   170,   353,
     172,   173,   174,   175,     0,   176,   354,   178,   355,   180,
     181,   182,   183,   184,   185,   186,   356,   357,   189,   190,
       0,  1353,     0,     0,   962,   963,   964,   965,     0,   966,
       0,     0,   967,     0,     0,   968,   969,   970,   971,   972,
     973,   974,   975,   976,     0,     0,   977,   978,   979,  1373,
       0,     0,   962,   963,   964,   965,     0,   966,     0,     0,
     967,     0,     0,   968,   969,   970,   971,   972,   973,   974,
     975,   976,     0,     0,   977,   978,   979,  1385,     0,     0,
     962,   963,   964,   965,     0,   966,   314,     0,   967,   980,
     981,   968,   969,   970,   971,   972,   973,   974,   975,   976,
       0,     0,   977,   978,   979,  1394,     0,     0,   962,   963,
     964,   965,     0,   966,   314,     0,   967,   980,   981,   968,
     969,   970,   971,   972,   973,   974,   975,   976,     0,     0,
     977,   978,   979,  1396,     0,     0,   962,   963,   964,   965,
       0,   966,   314,     0,   967,   980,   981,   968,   969,   970,
     971,   972,   973,   974,   975,   976,     0,     0,   977,   978,
     979,  1399,     0,     0,   962,   963,   964,   965,     0,   966,
     314,     0,   967,   980,   981,   968,   969,   970,   971,   972,
     973,   974,   975,   976,     0,     0,   977,   978,   979,  1400,
       0,     0,   962,   963,   964,   965,     0,   966,   314,     0,
     967,   980,   981,   968,   969,   970,   971,   972,   973,   974,
     975,   976,     0,     0,   977,   978,   979,  1405,     0,     0,
     962,   963,   964,   965,     0,   966,   314,     0,   967,   980,
     981,   968,   969,   970,   971,   972,   973,   974,   975,   976,
       0,     0,   977,   978,   979,  1408,     0,     0,   962,   963,
     964,   965,     0,   966,   314,     0,   967,   980,   981,   968,
     969,   970,   971,   972,   973,   974,   975,   976,     0,     0,
     977,   978,   979,  1411,     0,     0,   962,   963,   964,   965,
       0,   966,   314,     0,   967,   980,   981,   968,   969,   970,
     971,   972,   973,   974,   975,   976,     0,     0,   977,   978,
     979,  1413,     0,     0,   962,   963,   964,   965,     0,   966,
     314,     0,   967,   980,   981,   968,   969,   970,   971,   972,
     973,   974,   975,   976,     0,     0,   977,   978,   979,  1415,
       0,     0,   962,   963,   964,   965,     0,   966,   314,     0,
     967,   980,   981,   968,   969,   970,   971,   972,   973,   974,
     975,   976,     0,     0,   977,   978,   979,  1501,     0,     0,
     962,   963,   964,   965,     0,   966,   314,     0,   967,   980,
     981,   968,   969,   970,   971,   972,   973,   974,   975,   976,
       0,     0,   977,   978,   979,  1502,     0,     0,   962,   963,
     964,   965,     0,   966,   314,     0,   967,   980,   981,   968,
     969,   970,   971,   972,   973,   974,   975,   976,     0,     0,
     977,   978,   979,  1507,     0,     0,   962,   963,   964,   965,
       0,   966,   314,     0,   967,   980,   981,   968,   969,   970,
     971,   972,   973,   974,   975,   976,     0,     0,   977,   978,
     979,  1513,     0,     0,   962,   963,   964,   965,     0,   966,
     314,     0,   967,   980,   981,   968,   969,   970,   971,   972,
     973,   974,   975,   976,     0,     0,   977,   978,   979,  1569,
       0,     0,   962,   963,   964,   965,     0,   966,   314,     0,
     967,   980,   981,   968,   969,   970,   971,   972,   973,   974,
     975,   976,     0,     0,   977,   978,   979,  1570,     0,     0,
     962,   963,   964,   965,     0,   966,   314,     0,   967,   980,
     981,   968,   969,   970,   971,   972,   973,   974,   975,   976,
       0,     0,   977,   978,   979,  1573,     0,     0,   962,   963,
     964,   965,     0,   966,   314,     0,   967,   980,   981,   968,
     969,   970,   971,   972,   973,   974,   975,   976,     0,     0,
     977,   978,   979,  1574,     0,     0,   962,   963,   964,   965,
       0,   966,   314,     0,   967,   980,   981,   968,   969,   970,
     971,   972,   973,   974,   975,   976,  1158,     0,   977,   978,
     979,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     314,     0,     0,   980,   981,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  1159,     0,  1160,  1161,
    1162,  1163,  1164,  1165,  1166,     0,     0,     0,   314,     0,
       0,   980,   981,  1167,  1168,  1169,  1170,  1171,  1172,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  1173,  1174,
    1175
};

static const yytype_int16 yycheck[] =
{
      21,   229,   579,   921,  1059,   735,   736,   927,   236,  1081,
     381,   472,   646,  1140,     8,     8,  1326,    10,   476,   477,
     478,   479,   393,   394,   504,   312,   409,   622,   298,   624,
     472,   414,   627,   298,   417,   312,    10,   298,   421,   304,
     679,   680,   681,   304,   472,   472,   312,   312,   268,   298,
     270,   312,     3,     4,     5,   304,   304,   508,   509,   510,
     431,   512,   513,   312,   312,   298,   304,   304,   304,    66,
     521,   915,   123,   524,   312,   312,   312,  1308,   529,   530,
     531,   532,   533,   534,   535,   536,    14,   538,     9,    62,
     701,   702,   543,   544,   545,   298,   547,   548,    35,    62,
     645,   304,   304,   554,    53,   556,   123,    10,   559,   312,
     312,   115,    43,   953,    12,   955,   567,   568,   569,   570,
     571,   572,   573,   574,     9,   576,   123,    50,    51,    42,
     298,    35,   304,   123,    65,    11,   304,  1471,    12,     9,
     312,   152,   243,    42,   312,   985,    35,   102,   120,    42,
     242,    50,    51,    12,   274,   995,   701,   702,    36,    37,
    1000,   126,  1002,   893,  1004,     9,  1006,    62,  1008,     9,
    1010,   150,  1012,    50,    51,   905,  1016,   297,  1018,   183,
     728,    50,    51,   126,   106,    42,   298,  1310,   918,   144,
      50,    51,   304,   304,    51,   287,  1114,    73,   274,    43,
     312,   312,   115,    43,   183,   297,   307,     0,   173,   131,
     105,   222,   223,   761,   675,   435,   677,   108,   183,   439,
     231,   297,   115,   443,   444,   445,    23,   176,   448,   152,
     173,    89,    90,   675,   227,   677,   831,   230,   274,   211,
     212,   126,   625,   273,   148,   725,    43,   675,   675,   677,
     677,    32,   175,   227,   227,  1589,   230,    38,   641,   148,
     191,   312,   132,   314,   227,   301,   164,   297,   125,   720,
     183,   541,   175,   194,   645,   152,   541,   497,   565,   171,
     541,   501,   219,   161,  1594,   177,   162,   163,   565,   834,
     164,  1135,   541,   151,   222,   312,   313,   562,   175,   565,
     565,   562,   161,  1143,   565,   164,   175,  1147,   541,  1540,
      50,    51,   130,   562,   562,   175,   565,   565,   315,   316,
     317,    42,   312,   313,   562,   562,   562,   565,   565,   565,
     701,   702,   703,   704,    55,  1462,   242,   106,   541,   213,
     214,   215,   241,   249,  1184,   203,   204,    34,   299,   207,
    1473,  1474,  1475,  1476,   305,    35,  1196,   308,    42,   562,
     562,   382,   565,   565,    51,   386,    19,  1097,   226,   390,
    1100,  1101,    52,   541,    89,   396,   116,    92,    93,    94,
      95,   375,   375,    42,   120,  1115,  1441,   408,   381,    42,
     562,   412,   413,   565,   562,   389,   389,   565,    51,   619,
     393,   394,    71,    72,   425,   426,   427,  1327,    42,   629,
     630,   872,  1484,   874,    11,   409,   409,     9,    19,    42,
     414,   414,   793,   417,   417,  1036,    23,   421,   421,   541,
     872,  1070,   874,    19,   103,   655,   916,  1048,   431,    36,
     920,    42,  1019,  1020,   872,   872,   874,   874,   442,   442,
     562,   562,    47,   565,   565,    50,    51,    35,   829,    49,
      50,    51,    42,   834,    56,    57,    58,   488,   183,   205,
    1104,    51,   125,   225,    52,   211,   212,  1111,   274,   472,
     265,    26,    35,    47,    29,    30,    50,    51,    33,   241,
     286,  1036,    19,    12,   714,   715,   716,   273,   274,    52,
     296,  1096,    19,  1048,  1344,    50,    51,    49,    50,    51,
     504,   504,   297,   297,    19,   898,   300,   302,   303,   295,
     296,   116,   298,    42,    35,  1597,  1366,   117,    19,   138,
    1141,    50,    51,   274,   126,  1146,   145,    51,    35,   274,
    1151,    52,  1382,    19,  1155,   125,   274,  1467,  1468,   284,
    1390,    22,   116,    35,  1394,    52,   297,    71,    72,   300,
     301,   296,   297,  1481,    35,  1485,  1486,    42,   296,   297,
      52,   592,    35,   118,   119,   117,    51,   274,    68,    69,
      70,   274,   191,   192,   193,    35,   607,   284,   285,    52,
     810,  1202,    22,  1323,  1324,    35,  1141,    19,  1232,   296,
     297,  1146,    52,   296,   297,    35,  1151,   242,    27,   244,
    1155,    42,    52,   634,   635,  1073,   251,   252,   272,   186,
      51,  1079,   257,   190,    35,    44,  1087,   281,   848,   849,
     850,   625,   625,    35,   292,   270,   271,   127,   858,  1559,
     298,    52,    19,  1094,  1095,  1087,   304,   641,   641,    35,
      52,    51,   645,    35,   312,   676,    35,  1202,   274,  1087,
    1087,    35,   220,   883,    19,  1036,    52,    35,   284,   285,
      52,   229,    35,    52,  1308,    43,    19,  1048,    52,  1599,
     296,   297,   675,   173,   677,   208,   209,   210,   709,    52,
     242,   181,   182,   183,   184,   185,   180,   249,  1332,   180,
      32,    19,   192,   193,   188,  1316,    38,   188,   701,   702,
     703,   704,     9,   180,  1444,   457,   458,   459,   460,   461,
     462,   188,   464,   465,   466,   467,   468,   469,   470,   471,
      35,   725,   725,    32,   224,  1187,   730,   730,    43,    38,
      35,   734,   232,   180,    35,   235,  1198,  1199,    43,   237,
      35,   188,    43,   241,     9,  1207,    89,    90,    43,  1211,
      42,  1213,  1214,    96,  1216,    35,    35,  1219,  1220,    35,
    1141,  1316,    51,    43,    43,  1146,    51,    43,    35,    35,
    1151,    35,   115,    35,  1155,    35,    43,    43,   121,    43,
      35,    43,     9,    43,    35,    35,    42,    46,    43,    48,
     793,    19,    43,    43,    46,   133,    48,   135,   136,  1029,
     138,   139,   123,   124,  1034,  1035,  1274,  1275,   151,  1039,
     165,   166,  1433,  1434,   845,   846,    62,  1461,    56,    57,
      58,  1202,   213,   214,   215,    68,   829,    70,    42,  1416,
    1417,   834,  1437,   235,  1064,  1479,     3,     4,     5,  1460,
     138,   139,  1235,    68,  1237,    70,   308,   309,  1241,   853,
     853,    50,    51,   196,   197,   198,   199,   240,   201,   202,
     203,   204,  1092,  1093,   207,  1258,   106,   107,  1489,   872,
      89,   874,    35,    92,    93,    94,    95,  1498,  1433,  1434,
     164,   156,   166,   226,   127,   189,   190,    36,    37,  1510,
     171,   172,   121,   122,   898,   898,  1540,   171,   172,   132,
    1521,   195,   127,   157,  1548,  1460,   169,  1528,   912,   912,
     192,   193,   916,   916,   666,   160,   920,   920,   123,   124,
     140,   141,  1566,   158,    68,    69,    70,    87,    88,   168,
     173,    42,  1519,   190,  1489,  1316,   173,   940,   181,   182,
     183,   184,   185,  1498,    42,    68,    69,    70,   173,   192,
     193,    87,    88,   282,   283,  1510,   181,   182,   183,   184,
     185,    42,  1606,  1607,  1608,  1609,  1521,   192,   193,   187,
    1614,   310,   311,  1528,    13,    14,    15,    16,    17,    18,
      42,  1449,  1450,   127,   287,   288,    42,    26,   100,   101,
     216,   217,   235,   143,   144,    42,  1467,   178,   179,   224,
      39,   254,   255,    31,   127,   143,   144,   232,  1238,    51,
     235,     3,     4,     5,     6,     7,     8,   473,   474,   480,
     481,    16,    17,   482,   483,    43,    12,  1257,    43,   173,
     703,   704,    51,  1036,   491,   492,  1429,   181,   182,   183,
     184,   185,  1601,  1602,    43,  1048,    35,     9,   192,   193,
     173,    35,  1433,  1434,    51,    51,    42,   152,   181,   182,
     183,   184,   185,   159,    50,    51,   137,  1535,   138,   159,
     109,   110,   111,   112,   113,   114,  1314,   152,   152,  1460,
     152,    21,    56,   126,  1087,    53,   106,    43,    43,    43,
     107,   235,    43,    43,    43,    10,   225,    62,   152,   225,
     137,   224,    68,    91,    70,    24,    86,  1110,  1489,   232,
      42,    25,   235,    39,   126,     9,   132,  1498,   131,    10,
    1591,  1152,   115,   134,   150,  1156,  1110,   194,     9,  1510,
     154,   170,    42,    42,    42,    42,    42,  1140,  1141,  1591,
    1521,    43,    35,  1146,   213,    98,    23,  1528,  1151,  1180,
      97,   134,  1155,   231,  1185,  1186,  1140,   126,   231,  1190,
      39,   127,    35,   142,   173,   173,  1197,   126,   236,   191,
     115,   254,  1203,   297,  1205,   274,   242,  1208,   126,   299,
     297,  1212,   242,   297,    43,     9,  1217,     9,    43,    10,
      43,    43,   225,  1224,  1424,     9,   225,  1228,  1428,  1202,
    1430,  1431,    97,   142,    35,    39,    57,   173,    35,   173,
      39,   213,   213,   251,    94,   181,   182,   183,   184,   185,
     283,   282,   123,   299,   155,  1455,   266,    45,   242,    45,
     134,  1235,  1235,  1237,  1237,   132,    43,  1241,  1241,   152,
      42,   152,   145,   200,    19,   236,    42,    42,    21,    12,
     213,   123,   242,    21,  1258,  1258,   242,   242,   224,   245,
     246,   247,   248,   152,   250,   251,   232,   253,   152,   235,
     256,   257,   258,   259,   260,   261,   262,   263,   264,    10,
      35,   267,   268,   269,   132,    43,   206,   273,   274,    28,
     132,    43,   115,    21,   206,   951,     6,  1175,  1301,  1193,
    1182,   963,  1469,   306,   308,   299,   293,  1310,   308,   295,
     296,   293,   298,  1316,   832,  1296,   294,  1301,   299,  1031,
     196,   307,   196,   300,   310,   311,  1310,   623,   453,   305,
     305,  1064,   858,   626,  1365,   100,  1037,   628,   311,   455,
     475,   456,   501,   242,   219,  1418,   245,   246,   247,   248,
     141,   250,   432,  1583,   253,  1585,  1232,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   834,   704,   267,   268,
     269,   242,   393,   431,   245,   246,   247,   248,   829,   250,
     387,  1611,   253,   872,   874,   256,   257,   258,   259,   260,
     261,   262,   263,   264,  1087,   873,   267,   268,   269,   677,
    1483,  1488,   675,  1276,  1078,  1083,   618,  1300,   307,  1104,
    1438,   310,   311,    -1,    -1,   387,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,  1429,  1429,    -1,    -1,    -1,
    1433,  1434,    -1,    -1,    -1,    -1,   307,    -1,    -1,   310,
     311,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1460,    -1,  1462,
      -1,    -1,  1465,  1466,    -1,    -1,  1469,    -1,  1499,    -1,
    1473,  1474,  1475,  1476,    -1,    -1,    -1,    -1,  1462,    -1,
    1511,  1465,  1466,    -1,    -1,  1469,  1489,    -1,    -1,  1473,
    1474,  1475,  1476,    -1,    -1,  1498,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,  1510,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,  1521,    -1,
      -1,    -1,  1553,    -1,    -1,  1528,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    -1,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,    -1,    -1,    -1,   109,   110,   111,   112,
     113,   114,   115,    -1,    -1,    -1,    -1,    -1,   121,   122,
     123,    -1,    -1,    -1,   127,    -1,    -1,    -1,    -1,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,    -1,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,    -1,   187,   188,   189,    -1,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,    -1,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   245,
     246,   247,   248,    -1,   250,    -1,    -1,   253,    -1,    -1,
     256,   257,   258,   259,   260,   261,   262,   263,   264,    -1,
      -1,   267,   268,   269,    -1,     9,    -1,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    32,    -1,
      34,    -1,    36,    37,    38,    -1,    40,    41,    42,    43,
      -1,   307,    -1,    -1,   310,   311,    -1,    -1,    -1,   312,
      -1,   314,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    -1,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,    -1,    -1,    -1,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,    -1,    -1,    -1,   121,   122,   123,
      -1,    -1,   126,   127,   128,   129,    -1,    -1,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,    -1,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,    -1,   187,   188,   189,    -1,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,    -1,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
      -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,    29,    30,
      -1,    32,    -1,    34,    -1,    36,    37,    38,    -1,    40,
      41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    52,    -1,    -1,    -1,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,    -1,    -1,    -1,   109,   110,
     111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,    -1,
     121,   122,   123,    -1,    -1,   126,   127,   128,   129,    -1,
      -1,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
      -1,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,    -1,   187,   188,   189,    -1,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,    -1,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    32,    -1,    34,    -1,    36,    37,
      38,    -1,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,    -1,    -1,
      -1,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      -1,    -1,    -1,   121,   122,   123,    -1,    -1,   126,   127,
     128,   129,    -1,    -1,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,    -1,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,    -1,   187,
     188,   189,    -1,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,    -1,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,     9,    -1,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,
      -1,    26,    -1,    -1,    29,    30,    -1,    32,    -1,    34,
      -1,    36,    37,    38,    -1,    40,    41,    42,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    -1,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,    -1,    -1,    -1,    -1,    -1,   121,   122,   123,    -1,
      -1,   126,   127,   128,   129,    -1,    -1,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,    -1,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,    -1,   187,   188,   189,    -1,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,    -1,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,     9,    -1,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,    -1,
      -1,    -1,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      32,    -1,    34,    -1,    36,    37,    38,    -1,    40,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      52,    -1,    -1,    -1,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    -1,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,    -1,    -1,    -1,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,    -1,   121,
     122,   123,    -1,    -1,   126,   127,   128,   129,    -1,    -1,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,    -1,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,    -1,   187,   188,   189,    -1,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,    -1,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
       9,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    20,    -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,
      29,    30,    -1,    32,    -1,    34,    -1,    36,    37,    38,
      -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    52,    -1,    -1,    -1,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    -1,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,    -1,    -1,    -1,
     109,   110,   111,   112,   113,   114,   115,    -1,    -1,    -1,
      -1,    -1,   121,   122,   123,    -1,    -1,   126,   127,   128,
     129,    -1,    -1,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,    -1,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,    -1,   187,   188,
     189,    -1,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,    -1,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,     9,    -1,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,
      26,    -1,    -1,    29,    30,    -1,    32,    -1,    34,    -1,
      36,    37,    38,    -1,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
      -1,    -1,    -1,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,    -1,   121,   122,   123,    -1,    -1,
     126,   127,   128,   129,    -1,    -1,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,    -1,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
      -1,   187,   188,   189,    -1,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
      -1,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,     9,    -1,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,    -1,    -1,
      -1,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,    32,
      -1,    34,    -1,    36,    37,    38,    -1,    40,    41,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    -1,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,    -1,    -1,    -1,   109,   110,   111,   112,
     113,   114,   115,    -1,    -1,    -1,    -1,    -1,   121,   122,
     123,    -1,    -1,   126,   127,   128,   129,    -1,    -1,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,    -1,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,    -1,   187,   188,   189,    -1,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,    -1,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,     9,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    32,    -1,    34,    -1,    36,    37,    38,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
      -1,   121,   122,   123,    -1,    -1,   126,   127,   128,   129,
      -1,    -1,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,    -1,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,    -1,   187,   188,   189,
      -1,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,    -1,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    26,
      -1,    -1,    29,    30,    -1,    32,    -1,    34,    -1,    36,
      37,    38,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    -1,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,    -1,
      -1,    -1,   109,   110,   111,   112,   113,   114,   115,    -1,
      -1,    -1,    -1,    -1,   121,   122,   123,    -1,    -1,   126,
     127,   128,   129,    -1,    -1,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,    -1,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,    -1,
     187,   188,   189,    -1,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,    -1,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,     9,    -1,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    32,    -1,
      34,    -1,    36,    37,    38,    -1,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    -1,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,    -1,    -1,    -1,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,    -1,    -1,    -1,   121,   122,   123,
      -1,    -1,   126,   127,   128,   129,    -1,    -1,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,    -1,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,    -1,   187,   188,   189,    -1,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,    -1,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
      -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,    29,    30,
      -1,    32,    -1,    34,    -1,    36,    37,    38,    -1,    40,
      41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,    -1,    -1,    -1,   109,   110,
     111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,    -1,
     121,   122,   123,    -1,    -1,   126,   127,   128,   129,    -1,
      -1,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
      -1,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,    -1,   187,   188,   189,    -1,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,    -1,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    -1,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,    -1,    -1,
      -1,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      -1,    -1,    -1,   121,   122,   123,    -1,    -1,   126,   127,
     128,   129,    -1,    -1,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,    -1,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,    -1,   187,
     188,   189,    -1,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,     9,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      -1,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,    -1,    -1,    -1,   109,   110,
     111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,    -1,
     121,   122,   123,    -1,    -1,    -1,   127,    -1,    -1,    -1,
      -1,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
      -1,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,    -1,   187,   188,   189,    -1,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,     9,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    40,    -1,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,
      54,    -1,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    -1,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,    -1,    -1,    -1,   109,   110,   111,   112,   113,
     114,   115,    -1,    -1,    -1,    -1,    -1,   121,   122,   123,
      -1,    -1,   126,   127,    -1,    -1,    -1,    -1,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,    -1,   150,    -1,   152,    -1,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
      -1,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,    -1,   187,   188,   189,    -1,    -1,   192,    -1,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,     9,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    40,    -1,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    51,    -1,    -1,    -1,    -1,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    -1,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,    -1,
      -1,    -1,   109,   110,   111,   112,   113,   114,   115,    -1,
      -1,    -1,    -1,    -1,   121,   122,   123,    -1,    -1,   126,
     127,    -1,    -1,    -1,    -1,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,    -1,   150,    -1,   152,    -1,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,    -1,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,    -1,
     187,   188,   189,    -1,    -1,   192,    -1,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,    -1,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    -1,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
      -1,    -1,    -1,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,    -1,   121,   122,   123,    -1,    -1,
      -1,   127,   128,   129,    -1,    -1,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,    -1,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
      -1,   187,   188,   189,    -1,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
      -1,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    -1,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,    -1,    -1,    -1,    -1,    -1,   121,   122,   123,    -1,
      -1,    -1,   127,   128,   129,    -1,    -1,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,    -1,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,    -1,   187,   188,   189,    -1,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,    11,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
      -1,   121,   122,   123,    -1,    -1,    -1,   127,   128,   129,
      -1,    -1,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,    -1,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,    -1,   187,   188,   189,
      -1,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,    11,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    -1,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,    -1,    -1,    -1,   109,   110,   111,   112,   113,   114,
     115,    -1,    -1,    -1,    -1,    -1,   121,   122,   123,    -1,
      -1,    -1,   127,    -1,    -1,    -1,    -1,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,    -1,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,    -1,   187,   188,   189,    -1,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,    11,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    43,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
      -1,   121,   122,   123,    -1,    -1,    -1,   127,    -1,    -1,
      -1,    -1,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,    -1,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,    -1,   187,   188,   189,
      -1,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,    -1,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    -1,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,    -1,    -1,    -1,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
      -1,   121,   122,   123,    -1,    -1,    -1,   127,    -1,    -1,
      -1,    -1,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,    -1,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,    -1,   187,   188,   189,
      -1,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,    42,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,    -1,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
      -1,    -1,    -1,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,    -1,   121,   122,   123,    -1,    -1,
      -1,   127,    -1,    -1,    -1,    -1,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,    -1,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
      -1,   187,   188,   189,    -1,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
      42,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,    -1,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    -1,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,    -1,    -1,    -1,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,    -1,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,    -1,    -1,    -1,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,    -1,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,    -1,   187,   188,   189,    -1,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,    43,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    -1,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,    -1,    -1,    -1,
     109,   110,   111,   112,   113,   114,   115,    -1,    -1,    -1,
      -1,    -1,   121,   122,   123,    -1,    -1,   126,   127,    -1,
      -1,    -1,    -1,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,    -1,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,    -1,   187,   188,
     189,    -1,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,    -1,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    -1,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
      -1,    -1,    -1,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,    -1,   121,   122,   123,    -1,    -1,
      -1,   127,    -1,    -1,    -1,    -1,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,    -1,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
      -1,   187,   188,   189,    -1,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
      -1,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    -1,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,    -1,    -1,    -1,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,    -1,   121,
     122,   123,    -1,    -1,    -1,   127,    -1,    -1,    -1,    -1,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,    -1,   150,    -1,
     152,    -1,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,    -1,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,    -1,   187,   188,   189,    -1,    -1,
     192,    -1,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,    -1,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
      -1,   242,    -1,    -1,   245,   246,   247,   248,    -1,   250,
      -1,    -1,   253,    -1,    -1,   256,   257,   258,   259,   260,
     261,   262,   263,   264,    -1,    -1,   267,   268,   269,   242,
      -1,    -1,   245,   246,   247,   248,    -1,   250,    -1,    -1,
     253,    -1,    -1,   256,   257,   258,   259,   260,   261,   262,
     263,   264,    -1,    -1,   267,   268,   269,   242,    -1,    -1,
     245,   246,   247,   248,    -1,   250,   307,    -1,   253,   310,
     311,   256,   257,   258,   259,   260,   261,   262,   263,   264,
      -1,    -1,   267,   268,   269,   242,    -1,    -1,   245,   246,
     247,   248,    -1,   250,   307,    -1,   253,   310,   311,   256,
     257,   258,   259,   260,   261,   262,   263,   264,    -1,    -1,
     267,   268,   269,   242,    -1,    -1,   245,   246,   247,   248,
      -1,   250,   307,    -1,   253,   310,   311,   256,   257,   258,
     259,   260,   261,   262,   263,   264,    -1,    -1,   267,   268,
     269,   242,    -1,    -1,   245,   246,   247,   248,    -1,   250,
     307,    -1,   253,   310,   311,   256,   257,   258,   259,   260,
     261,   262,   263,   264,    -1,    -1,   267,   268,   269,   242,
      -1,    -1,   245,   246,   247,   248,    -1,   250,   307,    -1,
     253,   310,   311,   256,   257,   258,   259,   260,   261,   262,
     263,   264,    -1,    -1,   267,   268,   269,   242,    -1,    -1,
     245,   246,   247,   248,    -1,   250,   307,    -1,   253,   310,
     311,   256,   257,   258,   259,   260,   261,   262,   263,   264,
      -1,    -1,   267,   268,   269,   242,    -1,    -1,   245,   246,
     247,   248,    -1,   250,   307,    -1,   253,   310,   311,   256,
     257,   258,   259,   260,   261,   262,   263,   264,    -1,    -1,
     267,   268,   269,   242,    -1,    -1,   245,   246,   247,   248,
      -1,   250,   307,    -1,   253,   310,   311,   256,   257,   258,
     259,   260,   261,   262,   263,   264,    -1,    -1,   267,   268,
     269,   242,    -1,    -1,   245,   246,   247,   248,    -1,   250,
     307,    -1,   253,   310,   311,   256,   257,   258,   259,   260,
     261,   262,   263,   264,    -1,    -1,   267,   268,   269,   242,
      -1,    -1,   245,   246,   247,   248,    -1,   250,   307,    -1,
     253,   310,   311,   256,   257,   258,   259,   260,   261,   262,
     263,   264,    -1,    -1,   267,   268,   269,   242,    -1,    -1,
     245,   246,   247,   248,    -1,   250,   307,    -1,   253,   310,
     311,   256,   257,   258,   259,   260,   261,   262,   263,   264,
      -1,    -1,   267,   268,   269,   242,    -1,    -1,   245,   246,
     247,   248,    -1,   250,   307,    -1,   253,   310,   311,   256,
     257,   258,   259,   260,   261,   262,   263,   264,    -1,    -1,
     267,   268,   269,   242,    -1,    -1,   245,   246,   247,   248,
      -1,   250,   307,    -1,   253,   310,   311,   256,   257,   258,
     259,   260,   261,   262,   263,   264,    -1,    -1,   267,   268,
     269,   242,    -1,    -1,   245,   246,   247,   248,    -1,   250,
     307,    -1,   253,   310,   311,   256,   257,   258,   259,   260,
     261,   262,   263,   264,    -1,    -1,   267,   268,   269,   242,
      -1,    -1,   245,   246,   247,   248,    -1,   250,   307,    -1,
     253,   310,   311,   256,   257,   258,   259,   260,   261,   262,
     263,   264,    -1,    -1,   267,   268,   269,   242,    -1,    -1,
     245,   246,   247,   248,    -1,   250,   307,    -1,   253,   310,
     311,   256,   257,   258,   259,   260,   261,   262,   263,   264,
      -1,    -1,   267,   268,   269,   242,    -1,    -1,   245,   246,
     247,   248,    -1,   250,   307,    -1,   253,   310,   311,   256,
     257,   258,   259,   260,   261,   262,   263,   264,    -1,    -1,
     267,   268,   269,   242,    -1,    -1,   245,   246,   247,   248,
      -1,   250,   307,    -1,   253,   310,   311,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   242,    -1,   267,   268,
     269,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     307,    -1,    -1,   310,   311,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   272,    -1,   274,   275,
     276,   277,   278,   279,   280,    -1,    -1,    -1,   307,    -1,
      -1,   310,   311,   289,   290,   291,   292,   293,   294,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   304,   305,
     306
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,   319,     9,    11,
      20,    26,    29,    30,    32,    34,    36,    37,    38,    40,
      41,    42,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   109,   110,   111,   112,   113,   114,   115,   121,   122,
     123,   126,   127,   128,   129,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   187,   188,
     189,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   453,   454,   455,   456,   457,   458,   459,   464,   474,
     477,   481,   482,   483,   484,   486,   487,   488,   489,   490,
     491,   492,   493,   494,   495,   496,   497,   498,   499,   500,
     501,   503,   504,   505,   506,   507,   508,   509,   510,   511,
     512,   513,   514,   515,   516,   517,   519,   520,   521,   522,
     524,   525,   526,   527,   528,   530,   531,   532,   543,   545,
     547,   548,   549,   552,   553,   554,   555,   565,   566,   567,
     568,   569,   570,   571,   573,   575,   577,   612,   613,   614,
     615,   616,   617,   618,   621,   622,   623,   624,   626,   633,
     634,   638,   639,   640,   641,   645,   646,   186,   190,   404,
     405,   406,   407,   420,   423,   411,   412,   413,   414,   420,
     423,   415,   416,   417,   418,   420,   423,   419,   420,   421,
     422,   423,   424,   243,   307,   320,   321,   343,     0,    59,
      60,    61,    62,    63,    64,    65,    66,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,   104,
     121,   122,   133,   135,   146,   147,   149,   153,   167,   173,
     191,   193,   218,   221,   228,   230,   238,   239,   523,   645,
     646,    59,    60,    63,    66,   191,   514,   123,   124,   533,
     130,   108,   544,   505,   106,   535,   496,   496,    43,   454,
      19,    42,    51,   125,   550,    42,    51,    42,    42,    42,
      51,   125,   556,    42,    42,    42,    51,    19,    19,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    51,     9,
     619,   620,    51,    51,     9,   460,   461,     9,   194,   465,
     466,     9,   475,   476,   475,    42,    42,    51,    71,    72,
      19,    42,   550,    42,   448,    62,   105,    62,   227,    62,
      62,   227,   235,   240,    35,   132,   133,   135,   136,   138,
     139,   459,   464,   467,   468,   156,   157,    13,    14,    15,
      16,    17,    18,    26,    39,   109,   110,   111,   112,   113,
     114,   170,   195,    36,    37,   161,    11,    73,   162,   163,
      12,   164,   165,   166,   158,   169,   160,   168,    51,   501,
     505,    32,    38,   518,   514,   518,   514,    42,   518,   455,
     455,    42,    55,   187,   173,   180,   188,   408,   409,   426,
     428,   429,   430,   431,   433,   434,   435,   438,   439,   440,
     443,   444,   446,   453,   627,   405,   406,   407,   180,   408,
     409,   426,   428,   430,   431,   432,   444,   453,   627,   412,
     413,   414,   180,   425,   426,   428,   429,   430,   431,   433,
     434,   435,   438,   439,   444,   446,   447,   453,   611,   627,
     416,   417,   418,   421,   422,   424,   180,   425,   426,   428,
     430,   431,   432,   444,   447,   453,   627,   322,   344,   534,
     106,   107,   546,    31,   645,    43,    11,    43,   572,   579,
     626,   454,    51,   551,    43,   454,    43,    65,   191,   575,
     577,    43,    43,   126,   625,   645,   454,    51,   557,   574,
     579,   578,   580,   626,    43,   454,   454,   523,    35,   219,
     454,   454,   523,    35,   523,     9,    35,   523,    35,   148,
     148,   454,   454,   454,    51,    51,    11,    43,   576,   580,
     551,     9,    43,   450,   451,   152,   560,   455,   159,   455,
     645,   455,   455,   455,   455,   137,   138,   468,   483,   484,
     486,   486,   486,   486,   486,   486,   485,   486,   486,   486,
     486,   486,   486,   486,   486,    42,    51,    54,   520,   522,
     525,   528,   583,   585,   586,   587,   588,   589,   590,   646,
     489,   489,   488,   490,   490,   490,   490,   491,   491,   492,
     492,   159,   152,   152,   152,    52,   454,   506,   506,    21,
      43,   455,   529,   152,   222,   223,   231,    43,   529,    56,
     126,   625,    68,    70,   127,   173,   181,   182,   183,   184,
     185,   192,   193,   235,   410,   189,   190,    53,   427,   427,
     427,   427,   427,   427,   427,   427,   180,   188,    69,   427,
     427,   427,   427,   427,   427,   427,   427,   427,   180,   224,
     232,   449,   427,   427,   427,   427,   427,   427,   427,   427,
     180,   180,   427,   427,   427,   427,   427,   427,   427,   427,
     427,   272,   281,   323,   308,   309,   346,   106,   535,   107,
     106,   131,   502,    35,    43,    52,    52,   454,    52,    43,
      43,    43,    43,    52,    52,   454,    43,    43,    52,    52,
      10,   620,   455,    52,    52,   560,   461,   560,   523,   466,
     560,   476,   455,   455,    43,    43,    52,   454,   454,    35,
      43,   523,    35,    43,    42,    67,   174,   193,   561,   563,
     564,   565,   626,   642,   644,    50,    51,   452,   225,    62,
     152,   225,   225,   241,   455,   469,   470,   471,   137,   486,
     583,   454,   589,   208,   209,   210,   606,   606,   606,   606,
      91,   610,    24,   584,    25,    86,   561,   561,   558,   564,
     558,    52,   454,    35,    43,   220,   229,   455,   455,   455,
      43,   176,   427,    39,   171,   172,   126,   582,     9,   625,
     171,   172,   121,   122,   138,   145,   191,   410,   171,   177,
     436,   645,   237,   241,   630,   646,   173,   183,   442,   582,
     173,   582,    89,    90,   151,   203,   204,   207,   226,   592,
     593,   594,   595,   596,   599,   604,   605,    71,    72,   103,
     410,   242,   244,   251,   252,   257,   270,   271,   324,   325,
     328,   329,   333,   334,   335,   336,   337,   338,   341,   342,
     345,   242,   245,   246,   247,   248,   250,   253,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   267,   268,   269,
     310,   311,   343,   347,   348,   349,   350,   351,   352,   353,
     356,   357,   361,   362,   363,   364,   365,   370,   371,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   123,
     124,    27,    44,   131,   581,   626,    52,    52,   455,   132,
     115,   462,   194,   463,    10,   134,   150,   478,   480,   154,
      52,    52,   581,   560,   451,   563,    42,    42,    42,    11,
      23,    36,   562,   454,   454,   455,   455,   455,    42,    42,
     312,   314,   635,   645,    35,   140,   141,   472,   469,    43,
      52,    98,   213,    97,   586,    96,   115,   121,   196,   197,
     198,   199,   201,   202,   591,   592,   587,   134,    23,   559,
      22,   455,   231,   231,   126,   582,   523,    39,   142,   582,
     173,   173,    35,   126,   645,   123,   312,   313,   631,   632,
     236,   448,   625,   191,   582,   115,   441,   625,   441,    87,
      88,    89,    92,    93,    94,    95,    87,    88,   624,    89,
      92,    93,    94,    95,   183,   646,   282,   283,   273,   274,
     295,   296,   298,   330,   274,   286,   296,   330,   274,   284,
     285,   296,   297,   346,   274,   296,   297,   346,   242,   272,
     274,   275,   276,   277,   278,   279,   280,   289,   290,   291,
     292,   293,   294,   304,   305,   306,   242,   116,   452,   351,
     297,   254,   366,   367,   368,   299,   297,   300,   346,   273,
     297,   355,   274,   358,   242,   346,   265,   297,   302,   303,
     372,   274,   296,   297,   346,   297,   346,   274,   297,   346,
     346,   274,   297,   300,   301,   346,   274,   297,   346,   274,
     301,   346,   242,   287,   297,   287,   288,   346,   297,   346,
     535,   535,   542,    43,   455,     9,   463,     9,    10,   455,
     455,     9,   561,   183,   479,   480,   455,    23,    43,    43,
      43,    43,    43,   561,   643,    52,    52,   225,     9,   636,
     637,   636,   225,   470,   142,   473,   606,   490,    97,   100,
     101,   622,   490,   120,   211,   212,   607,   607,   216,   217,
     609,   609,   588,   455,   455,   427,   427,   560,   582,   143,
     144,   582,   582,   178,   179,   437,    42,   452,   629,   630,
      35,    20,    32,    38,    59,    74,   233,   234,   394,   395,
     396,   397,   400,   401,   402,   514,   152,   175,   452,    39,
     173,   441,   582,    35,    39,   213,    42,   115,   183,   598,
     213,    94,   448,   283,   282,   326,   394,   561,    56,    57,
      58,   327,   251,   331,   332,   346,   561,   346,   561,   454,
     242,   561,   454,   242,    66,   123,   315,   316,   317,   339,
     123,   340,   340,   242,   454,   299,   255,   367,   369,   346,
     454,   454,   345,   242,   123,   312,   314,   354,   454,   242,
     249,   359,   360,   242,   359,   242,   346,   454,   345,   345,
     266,   373,   561,   454,   242,   454,   242,   345,   454,   242,
     242,   345,   454,   345,   345,   242,   345,   454,   242,   345,
     345,   242,   454,   242,   454,   242,    45,    45,    33,   118,
     119,   452,   531,   523,   134,   523,   455,   523,   132,     9,
     132,   155,    43,   152,    35,    43,   455,   523,    35,    43,
      43,    42,   143,   144,   145,   213,   214,   215,   608,   102,
     144,   490,   490,   608,   200,    10,   175,   445,    43,   450,
     152,   628,   236,   632,   514,    19,    19,    42,    42,    12,
     452,   629,   401,    32,    38,    32,    38,   403,   518,   561,
     582,   582,   183,   598,   624,    42,   115,   601,   213,   152,
     175,   452,   123,   312,   313,   274,   284,   285,   296,   297,
     346,   242,   242,   123,   454,   346,   242,   242,   274,   284,
     296,   297,   346,   242,   346,   242,   346,   535,   535,   533,
     455,   152,   455,   523,   455,   455,   561,   561,   152,   560,
     637,   636,   582,   490,   490,   161,   455,    43,   561,   452,
     394,   514,   514,   398,   522,   624,   624,   395,   628,   401,
     401,   401,   401,    21,   175,   452,   441,   597,   597,   205,
     607,   602,   624,   624,   600,   600,   561,   561,   454,   242,
     242,   561,   454,   242,   242,    46,    48,   536,    46,    48,
     537,   535,   561,   132,   561,    10,    43,   490,   452,   629,
      43,    35,   452,   454,    35,    43,    43,   624,   206,    35,
      43,   164,   166,   603,   175,   452,   538,   540,   539,   541,
      28,   132,   455,   455,   628,   399,   520,   522,    22,   598,
     607,   624,   601,   601,    47,   116,   452,    49,   117,   452,
      47,   116,   452,    49,   117,   452,   455,   452,    43,   206
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
#if YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 8:
#line 762 "../src/parser/XQParser.y"
    {
    SequenceType *optionalString =
      WRAP((yylsp[(1) - (1)]), new (MEMMGR) SequenceType(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                         AnyAtomicType::fgDT_ANYATOMICTYPE,
                                         SequenceType::QUESTION_MARK, MEMMGR));

    XQGlobalVariable *nameVar =
      WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQGlobalVariable(0, optionalString,
                                             WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQSequence(MEMMGR)), MEMMGR, /*isParam*/true));
    nameVar->setVariableURI(XQillaFunction::XMLChFunctionURI);
    nameVar->setVariableLocalName(var_name);

    QP->_query->addVariable(nameVar);

    ASTNode *nameVarRef1 = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQVariable(XQillaFunction::XMLChFunctionURI, var_name, MEMMGR));
    XQCallTemplate *call = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQCallTemplate(nameVarRef1, 0, MEMMGR));

    ASTNode *ci = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQContextItem(MEMMGR));
    ASTNode *apply = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQApplyTemplates(ci, 0, 0, MEMMGR));

    VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
    args.push_back(WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQVariable(XQillaFunction::XMLChFunctionURI, var_name, MEMMGR)));
    FunctionExists *exists = WRAP((yylsp[(1) - (1)]), new (MEMMGR) FunctionExists(args, MEMMGR));

    QP->_query->setQueryBody(WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQIf(exists, call, apply, MEMMGR)));
  }
    break;

  case 9:
#line 792 "../src/parser/XQParser.y"
    {
  }
    break;

  case 10:
#line 795 "../src/parser/XQParser.y"
    {
    // TBD Check for xsl:version attr - jpcs
    NodeTest *nt = new (MEMMGR) NodeTest();
    nt->setNodeType(Node::document_string);
    nt->setNameWildcard();
    nt->setNamespaceWildcard();

    VectorOfASTNodes *pattern = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    pattern->push_back(WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQStep(XQStep::SELF, nt, MEMMGR)));

    XQUserFunction::ModeList *modelist = new (MEMMGR) XQUserFunction::ModeList(XQillaAllocator<XQUserFunction::Mode*>(MEMMGR));
    modelist->push_back(WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::DEFAULT)));

    XQUserFunction *func = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction(0, pattern, 0, (yyvsp[(1) - (1)].astNode), 0, MEMMGR));
    func->setModeList(modelist);

    QP->_query->addFunction(func);
  }
    break;

  case 12:
#line 818 "../src/parser/XQParser.y"
    {
    // TBD Check the value - jpcs
  }
    break;

  case 13:
#line 822 "../src/parser/XQParser.y"
    {
    // TBD implement exclude-result-prefixes - jpcs
  }
    break;

  case 21:
#line 840 "../src/parser/XQParser.y"
    {
    if((yyvsp[(1) - (4)].functDecl)->getName() == 0 && (yyvsp[(1) - (4)].functDecl)->getPattern() == 0) {
      yyerror((yylsp[(1) - (4)]), "The xsl:template declaration does not have either a {}name or {}match attribute, or both [err:XTSE0500]");
    }

    if((yyvsp[(1) - (4)].functDecl)->getPattern() != 0 && (yyvsp[(1) - (4)].functDecl)->getModeList() == 0) {
      XQUserFunction::ModeList *modelist = new (MEMMGR) XQUserFunction::ModeList(XQillaAllocator<XQUserFunction::Mode*>(MEMMGR));
      modelist->push_back(WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::DEFAULT)));
      (yyvsp[(1) - (4)].functDecl)->setModeList(modelist);
    }

    (yyvsp[(1) - (4)].functDecl)->setArgumentSpecs((yyvsp[(2) - (4)].argSpecs));
    (yyvsp[(1) - (4)].functDecl)->setFunctionBody((yyvsp[(3) - (4)].parenExpr));
    QP->_query->addFunction((yyvsp[(1) - (4)].functDecl));
  }
    break;

  case 22:
#line 859 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction(0, 0, 0, 0, 0, MEMMGR));
  }
    break;

  case 23:
#line 863 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = (yyvsp[(1) - (3)].functDecl);
    (yyval.functDecl)->setPattern((yyvsp[(3) - (3)].itemList));
  }
    break;

  case 24:
#line 868 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = (yyvsp[(1) - (2)].functDecl);
    RESOLVE_QNAME((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].str));
    (yyval.functDecl)->setURI(uri);
    (yyval.functDecl)->setName(name);
  }
    break;

  case 25:
#line 875 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = (yyvsp[(1) - (3)].functDecl);
    // TBD priority - jpcs
  }
    break;

  case 26:
#line 880 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = (yyvsp[(1) - (3)].functDecl);
    (yyval.functDecl)->setModeList((yyvsp[(3) - (3)].modeList));
  }
    break;

  case 27:
#line 885 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = (yyvsp[(1) - (3)].functDecl);
    (yyval.functDecl)->setReturnType((yyvsp[(3) - (3)].sequenceType));
  }
    break;

  case 28:
#line 893 "../src/parser/XQParser.y"
    {
    (yyval.modeList) = new (MEMMGR) XQUserFunction::ModeList(XQillaAllocator<XQUserFunction::Mode*>(MEMMGR));
  }
    break;

  case 29:
#line 897 "../src/parser/XQParser.y"
    {
    RESOLVE_QNAME((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].str));
    (yyvsp[(1) - (2)].modeList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQUserFunction::Mode(uri, name)));
    (yyval.modeList) = (yyvsp[(1) - (2)].modeList);
  }
    break;

  case 30:
#line 903 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (2)].modeList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::DEFAULT)));
    (yyval.modeList) = (yyvsp[(1) - (2)].modeList);
  }
    break;

  case 31:
#line 908 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (2)].modeList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::ALL)));
    (yyval.modeList) = (yyvsp[(1) - (2)].modeList);
  }
    break;

  case 35:
#line 918 "../src/parser/XQParser.y"
    {
    if((yyvsp[(1) - (4)].functDecl)->getName() == 0) {
      yyerror((yylsp[(1) - (4)]), "The xsl:function declaration does not have a {}name attribute");
    }

    (yyvsp[(1) - (4)].functDecl)->setArgumentSpecs((yyvsp[(2) - (4)].argSpecs));
    (yyvsp[(1) - (4)].functDecl)->setFunctionBody((yyvsp[(3) - (4)].parenExpr));
    QP->_query->addFunction((yyvsp[(1) - (4)].functDecl));
  }
    break;

  case 36:
#line 931 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction(0, 0, 0, 0, false, true, MEMMGR));
  }
    break;

  case 37:
#line 935 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = (yyvsp[(1) - (2)].functDecl);
    RESOLVE_QNAME((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].str));
    (yyval.functDecl)->setURI(uri);
    (yyval.functDecl)->setName(name);
  }
    break;

  case 38:
#line 942 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = (yyvsp[(1) - (3)].functDecl);
    (yyval.functDecl)->setReturnType((yyvsp[(3) - (3)].sequenceType));
  }
    break;

  case 39:
#line 947 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = (yyvsp[(1) - (2)].functDecl);
    // TBD override - jpcs
  }
    break;

  case 40:
#line 955 "../src/parser/XQParser.y"
    {
    (yyval.argSpecs) = new (MEMMGR) XQUserFunction::ArgumentSpecs(XQillaAllocator<XQUserFunction::ArgumentSpec*>(MEMMGR));
  }
    break;

  case 41:
#line 959 "../src/parser/XQParser.y"
    {
    (yyval.argSpecs) = (yyvsp[(1) - (2)].argSpecs);
    (yyval.argSpecs)->push_back((yyvsp[(2) - (2)].argSpec));
  }
    break;

  case 42:
#line 967 "../src/parser/XQParser.y"
    {
    (yyval.argSpec) = (yyvsp[(1) - (3)].argSpec);

    if((yyval.argSpec)->getName() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:param instruction does not have a {}name attribute");
    }

    if((yyval.argSpec)->getType() == 0) {
      (yyval.argSpec)->setType(WRAP((yylsp[(1) - (3)]), new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR)));
    }

    // TBD default parameter values - jpcs

/*     if(!$2->getChildren().empty()) { */
/*       if($$->value != 0) { */
/*         yyerror(@1, "The xsl:with-param instruction has both a select attribute and a sequence constructor [err:XTSE0870]"); */
/*       } */
/*       $$->value = $2; */
/*     } */
/*     else if($$->value == 0) { */
/*       yyerror(@1, "The xsl:with-param instruction has neither a select attribute nor a sequence constructor [err:XTSE0870]"); */
/*     } */
  }
    break;

  case 43:
#line 994 "../src/parser/XQParser.y"
    {
    (yyval.argSpec) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::ArgumentSpec(0, 0, MEMMGR));
  }
    break;

  case 44:
#line 998 "../src/parser/XQParser.y"
    {
    (yyval.argSpec) = (yyvsp[(1) - (2)].argSpec);
    RESOLVE_QNAME((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].str));
    (yyval.argSpec)->setURI(uri);
    (yyval.argSpec)->setName(name);
  }
    break;

  case 45:
#line 1005 "../src/parser/XQParser.y"
    {
    (yyval.argSpec) = (yyvsp[(1) - (3)].argSpec);
    // TBD default parameter values - jpcs
/*     $$->value = PRESERVE_NS(@2, $3); */
  }
    break;

  case 46:
#line 1011 "../src/parser/XQParser.y"
    {
    (yyval.argSpec) = (yyvsp[(1) - (3)].argSpec);
    (yyval.argSpec)->setType((yyvsp[(3) - (3)].sequenceType));
  }
    break;

  case 47:
#line 1016 "../src/parser/XQParser.y"
    {
    (yyval.argSpec) = (yyvsp[(1) - (2)].argSpec);
    // TBD required - jpcs
  }
    break;

  case 48:
#line 1021 "../src/parser/XQParser.y"
    {
    (yyval.argSpec) = (yyvsp[(1) - (2)].argSpec);
    // TBD tunnel parameters - jpcs
  }
    break;

  case 49:
#line 1030 "../src/parser/XQParser.y"
    {
    if((yyvsp[(1) - (3)].globalVar)->isRequired()) {
      if((yyvsp[(1) - (3)].globalVar)->getVariableExpr() != 0 || !(yyvsp[(2) - (3)].parenExpr)->getChildren().empty()) {
        yyerror((yylsp[(1) - (3)]), "A required xsl:param declaration must be empty and must not specify a {}select attribute");
      }
    }
    else {
      (yyvsp[(1) - (3)].globalVar)->setVariableExpr(XSLT_VARIABLE_VALUE((yylsp[(1) - (3)]), (ASTNode*)(yyvsp[(1) - (3)].globalVar)->getVariableExpr(), (yyvsp[(2) - (3)].parenExpr), (SequenceType*)(yyvsp[(1) - (3)].globalVar)->getSequenceType()));

      if((yyvsp[(1) - (3)].globalVar)->getVariableExpr() == 0) {
        yyerror((yylsp[(1) - (3)]), "The xsl:param declaration has both a select attribute and a sequence constructor [err:XTSE0620]");
      }
    }

    if((yyvsp[(1) - (3)].globalVar)->getVariableLocalName() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:param declaration does not have a {}name attribute");
    }

    QP->_query->addVariable((yyvsp[(1) - (3)].globalVar));
  }
    break;

  case 50:
#line 1054 "../src/parser/XQParser.y"
    {
    (yyval.globalVar) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQGlobalVariable(0, 0, 0, MEMMGR, /*isParam*/true));
  }
    break;

  case 51:
#line 1058 "../src/parser/XQParser.y"
    {
    RESOLVE_QNAME((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].str));
    (yyvsp[(1) - (2)].globalVar)->setVariableURI(uri);
    (yyvsp[(1) - (2)].globalVar)->setVariableLocalName(name);
    (yyval.globalVar) = (yyvsp[(1) - (2)].globalVar);
  }
    break;

  case 52:
#line 1065 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].globalVar)->setVariableExpr(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));
    (yyval.globalVar) = (yyvsp[(1) - (3)].globalVar);
  }
    break;

  case 53:
#line 1070 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].globalVar)->setSequenceType((yyvsp[(3) - (3)].sequenceType));
    (yyval.globalVar) = (yyvsp[(1) - (3)].globalVar);
  }
    break;

  case 54:
#line 1075 "../src/parser/XQParser.y"
    {
    (yyval.globalVar) = (yyvsp[(1) - (2)].globalVar);
    (yyval.globalVar)->setRequired((yyvsp[(2) - (2)].boolean));
  }
    break;

  case 55:
#line 1080 "../src/parser/XQParser.y"
    {
    (yyval.globalVar) = (yyvsp[(1) - (2)].globalVar);
    if((yyvsp[(2) - (2)].boolean)) {
      yyerror((yylsp[(2) - (2)]), "An xsl:param declaration cannot have a {}tunnel attribute with a value of \"yes\"");
    }
  }
    break;

  case 56:
#line 1091 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].globalVar)->setVariableExpr(XSLT_VARIABLE_VALUE((yylsp[(1) - (3)]), (ASTNode*)(yyvsp[(1) - (3)].globalVar)->getVariableExpr(), (yyvsp[(2) - (3)].parenExpr), (SequenceType*)(yyvsp[(1) - (3)].globalVar)->getSequenceType()));
    (yyvsp[(1) - (3)].globalVar)->setSequenceType(0);

    if((yyvsp[(1) - (3)].globalVar)->getVariableExpr() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:variable declaration has both a select attribute and a sequence constructor [err:XTSE0620]");
    }

    if((yyvsp[(1) - (3)].globalVar)->getVariableLocalName() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:variable declaration does not have a {}name attribute");
    }

    QP->_query->addVariable((yyvsp[(1) - (3)].globalVar));
  }
    break;

  case 57:
#line 1109 "../src/parser/XQParser.y"
    {
    (yyval.globalVar) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQGlobalVariable(0, 0, 0, MEMMGR));
  }
    break;

  case 58:
#line 1113 "../src/parser/XQParser.y"
    {
    RESOLVE_QNAME((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].str));
    (yyvsp[(1) - (2)].globalVar)->setVariableURI(uri);
    (yyvsp[(1) - (2)].globalVar)->setVariableLocalName(name);
    (yyval.globalVar) = (yyvsp[(1) - (2)].globalVar);
  }
    break;

  case 59:
#line 1120 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].globalVar)->setVariableExpr(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));
    (yyval.globalVar) = (yyvsp[(1) - (3)].globalVar);
  }
    break;

  case 60:
#line 1125 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].globalVar)->setSequenceType((yyvsp[(3) - (3)].sequenceType));
    (yyval.globalVar) = (yyvsp[(1) - (3)].globalVar);
  }
    break;

  case 61:
#line 1133 "../src/parser/XQParser.y"
    {
    // TBD Add the output to the static context - jpcs
  }
    break;

  case 62:
#line 1158 "../src/parser/XQParser.y"
    {
  }
    break;

  case 63:
#line 1161 "../src/parser/XQParser.y"
    {
  }
    break;

  case 64:
#line 1164 "../src/parser/XQParser.y"
    {
  }
    break;

  case 65:
#line 1167 "../src/parser/XQParser.y"
    {
  }
    break;

  case 66:
#line 1170 "../src/parser/XQParser.y"
    {
  }
    break;

  case 67:
#line 1173 "../src/parser/XQParser.y"
    {
  }
    break;

  case 68:
#line 1176 "../src/parser/XQParser.y"
    {
  }
    break;

  case 69:
#line 1179 "../src/parser/XQParser.y"
    {
  }
    break;

  case 70:
#line 1182 "../src/parser/XQParser.y"
    {
  }
    break;

  case 71:
#line 1185 "../src/parser/XQParser.y"
    {
  }
    break;

  case 72:
#line 1188 "../src/parser/XQParser.y"
    {
  }
    break;

  case 73:
#line 1191 "../src/parser/XQParser.y"
    {
  }
    break;

  case 74:
#line 1194 "../src/parser/XQParser.y"
    {
  }
    break;

  case 75:
#line 1197 "../src/parser/XQParser.y"
    {
  }
    break;

  case 76:
#line 1200 "../src/parser/XQParser.y"
    {
  }
    break;

  case 77:
#line 1203 "../src/parser/XQParser.y"
    {
  }
    break;

  case 78:
#line 1206 "../src/parser/XQParser.y"
    {
  }
    break;

  case 79:
#line 1209 "../src/parser/XQParser.y"
    {
  }
    break;

  case 84:
#line 1219 "../src/parser/XQParser.y"
    {
//     $$ = $1;
    const XMLCh *p = (yyvsp[(1) - (1)].str);
    while(*p && *p != ':') ++p;
    if(*p == 0) {
      yyerror((yylsp[(1) - (1)]), "The method for the xsl:output declaration does not have a prefix");
    }
  }
    break;

  case 85:
#line 1231 "../src/parser/XQParser.y"
    {
  }
    break;

  case 86:
#line 1234 "../src/parser/XQParser.y"
    {
  }
    break;

  case 88:
#line 1242 "../src/parser/XQParser.y"
    {
    LOCATION((yylsp[(1) - (1)]), loc);
    CONTEXT->addSchemaLocation(XMLUni::fgZeroLenString, 0, &loc);
  }
    break;

  case 89:
#line 1247 "../src/parser/XQParser.y"
    {
    LOCATION((yylsp[(1) - (2)]), loc);
    CONTEXT->addSchemaLocation((yyvsp[(2) - (2)].str), 0, &loc);
  }
    break;

  case 90:
#line 1252 "../src/parser/XQParser.y"
    {
    VectorOfStrings schemaLoc(XQillaAllocator<const XMLCh*>(MEMMGR));
    schemaLoc.push_back((yyvsp[(2) - (2)].str));

    LOCATION((yylsp[(1) - (2)]), loc);
    CONTEXT->addSchemaLocation(XMLUni::fgZeroLenString, &schemaLoc, &loc);
  }
    break;

  case 91:
#line 1260 "../src/parser/XQParser.y"
    {
    VectorOfStrings schemaLoc(XQillaAllocator<const XMLCh*>(MEMMGR));
    schemaLoc.push_back((yyvsp[(3) - (3)].str));

    LOCATION((yylsp[(1) - (3)]), loc);
    CONTEXT->addSchemaLocation((yyvsp[(2) - (3)].str), &schemaLoc, &loc);
  }
    break;

  case 92:
#line 1268 "../src/parser/XQParser.y"
    {
    VectorOfStrings schemaLoc(XQillaAllocator<const XMLCh*>(MEMMGR));
    schemaLoc.push_back((yyvsp[(2) - (3)].str));

    LOCATION((yylsp[(1) - (3)]), loc);
    CONTEXT->addSchemaLocation((yyvsp[(3) - (3)].str), &schemaLoc, &loc);
  }
    break;

  case 93:
#line 1279 "../src/parser/XQParser.y"
    {
    VectorOfASTNodes *children = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    *children = (yyvsp[(3) - (4)].parenExpr)->getChildren();

    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQElementConstructor((yyvsp[(1) - (4)].astNode), (yyvsp[(2) - (4)].itemList), children, MEMMGR));
  }
    break;

  case 94:
#line 1289 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR)); 
  }
    break;

  case 95:
#line 1293 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (3)].itemList);

    ASTNode *attrItem = WRAP((yylsp[(2) - (3)]), new (MEMMGR) XQAttributeConstructor((yyvsp[(2) - (3)].astNode), (yyvsp[(3) - (3)].itemList), MEMMGR));
    (yyval.itemList)->push_back(attrItem);
  }
    break;

  case 96:
#line 1300 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->insert((yyval.itemList)->begin(), (yyvsp[(2) - (2)].astNode));
  }
    break;

  case 97:
#line 1309 "../src/parser/XQParser.y"
    { 
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
  }
    break;

  case 98:
#line 1313 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back(PRESERVE_NS((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].astNode)));
  }
    break;

  case 99:
#line 1318 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQLiteral(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                               SchemaSymbols::fgDT_STRING,
                               (yyvsp[(2) - (2)].str), AnyAtomicType::STRING, MEMMGR)));
  }
    break;

  case 100:
#line 1328 "../src/parser/XQParser.y"
    {
    (yyval.parenExpr) = WRAP((yyloc), new (MEMMGR) XQSequence(MEMMGR));
  }
    break;

  case 101:
#line 1332 "../src/parser/XQParser.y"
    {
    (yyval.parenExpr) = (yyvsp[(1) - (2)].parenExpr);
    (yyval.parenExpr)->addItem(WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQTextConstructor((yyvsp[(2) - (2)].astNode), MEMMGR)));
  }
    break;

  case 102:
#line 1337 "../src/parser/XQParser.y"
    {
    (yyval.parenExpr) = (yyvsp[(1) - (2)].parenExpr);
    (yyval.parenExpr)->addItem((yyvsp[(2) - (2)].astNode));
  }
    break;

  case 103:
#line 1342 "../src/parser/XQParser.y"
    {
    (yyval.parenExpr) = (yyvsp[(1) - (2)].parenExpr);
    (yyval.parenExpr)->addItem((yyvsp[(2) - (2)].astNode));
  }
    break;

  case 122:
#line 1371 "../src/parser/XQParser.y"
    {
    XQTextConstructor *text = (XQTextConstructor*)(yyvsp[(1) - (3)].astNode);
    (yyval.astNode) = text;

    if(!(yyvsp[(2) - (3)].parenExpr)->getChildren().empty()) {
      if(text->getValue() != 0) {
        yyerror((yylsp[(1) - (3)]), "The xsl:value-of instruction has both a select attribute and a sequence constructor [err:XTSE0870]");
      }
      text->setValue((yyvsp[(2) - (3)].parenExpr));
    }
    else if(text->getValue() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:value-of instruction has neither a select attribute nor a sequence constructor [err:XTSE0870]");
    }
  }
    break;

  case 123:
#line 1389 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQTextConstructor(0, MEMMGR));
  }
    break;

  case 124:
#line 1393 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ((XQTextConstructor*)(yyval.astNode))->setValue(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));
  }
    break;

  case 125:
#line 1398 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    // TBD separator - jpcs
/*     ((XQTextConstructor*)$$)->setValue(PRESERVE_NS(@2, $3)); */
  }
    break;

  case 126:
#line 1407 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQTextConstructor((yyvsp[(2) - (3)].astNode), MEMMGR));
  }
    break;

  case 129:
#line 1416 "../src/parser/XQParser.y"
    {
    // TBD xsl:sort - jpcs
    XQApplyTemplates *apply = (XQApplyTemplates*)(yyvsp[(1) - (3)].astNode);
    (yyval.astNode) = apply;

    apply->setArguments((yyvsp[(2) - (3)].templateArgs));

    if(apply->getExpression() == 0) {
      NodeTest *nt = new (MEMMGR) NodeTest();
      nt->setTypeWildcard();
      nt->setNameWildcard();
      nt->setNamespaceWildcard();

      apply->setExpression(WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQStep(XQStep::CHILD, nt, MEMMGR)));
    }
  }
    break;

  case 130:
#line 1436 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQApplyTemplates(0, 0, 0, MEMMGR));
  }
    break;

  case 131:
#line 1440 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ((XQApplyTemplates*)(yyval.astNode))->setExpression(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));
  }
    break;

  case 132:
#line 1445 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ((XQApplyTemplates*)(yyval.astNode))->setMode((yyvsp[(3) - (3)].mode));
  }
    break;

  case 133:
#line 1453 "../src/parser/XQParser.y"
    {
    RESOLVE_QNAME((yylsp[(1) - (1)]), (yyvsp[(1) - (1)].str));
    (yyval.mode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode(uri, name));
  }
    break;

  case 134:
#line 1458 "../src/parser/XQParser.y"
    {
    (yyval.mode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::DEFAULT));
  }
    break;

  case 135:
#line 1462 "../src/parser/XQParser.y"
    {
    (yyval.mode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::CURRENT));
  }
    break;

  case 136:
#line 1469 "../src/parser/XQParser.y"
    {
    (yyval.templateArgs) = new (MEMMGR) TemplateArguments(XQillaAllocator<XQTemplateArgument*>(MEMMGR));
  }
    break;

  case 137:
#line 1473 "../src/parser/XQParser.y"
    {
    (yyval.templateArgs) = (yyvsp[(1) - (2)].templateArgs);
    (yyval.templateArgs)->push_back((yyvsp[(2) - (2)].templateArg));
  }
    break;

  case 138:
#line 1481 "../src/parser/XQParser.y"
    {
    XQCallTemplate *call = (XQCallTemplate*)(yyvsp[(1) - (3)].astNode);
    call->setArguments((yyvsp[(2) - (3)].templateArgs));
    (yyval.astNode) = call;

    if(call->getName() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:call-template instruction does not have a {}name attribute");
    }
  }
    break;

  case 139:
#line 1494 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQCallTemplate((const XMLCh*)0, 0, MEMMGR));
  }
    break;

  case 140:
#line 1498 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (2)].astNode);
    RESOLVE_QNAME((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].str));
    ((XQCallTemplate*)(yyval.astNode))->setURI(uri);
    ((XQCallTemplate*)(yyval.astNode))->setName(name);
  }
    break;

  case 141:
#line 1508 "../src/parser/XQParser.y"
    {
    (yyval.templateArgs) = new (MEMMGR) TemplateArguments(XQillaAllocator<XQTemplateArgument*>(MEMMGR));
  }
    break;

  case 142:
#line 1512 "../src/parser/XQParser.y"
    {
    (yyval.templateArgs) = (yyvsp[(1) - (2)].templateArgs);
    (yyval.templateArgs)->push_back((yyvsp[(2) - (2)].templateArg));
  }
    break;

  case 143:
#line 1520 "../src/parser/XQParser.y"
    {
    (yyval.templateArg) = (yyvsp[(1) - (3)].templateArg);

    if((yyval.templateArg)->name == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:with-param instruction does not have a {}name attribute");
    }

    (yyvsp[(1) - (3)].templateArg)->value = XSLT_VARIABLE_VALUE((yylsp[(1) - (3)]), (yyvsp[(1) - (3)].templateArg)->value, (yyvsp[(2) - (3)].parenExpr), (yyvsp[(1) - (3)].templateArg)->seqType);
    (yyvsp[(1) - (3)].templateArg)->seqType = 0;

    if((yyvsp[(1) - (3)].templateArg)->value == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:with-param instruction has both a select attribute and a sequence constructor [err:XTSE0870]");
    }
  }
    break;

  case 144:
#line 1538 "../src/parser/XQParser.y"
    {
    (yyval.templateArg) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQTemplateArgument(0, 0, MEMMGR));
  }
    break;

  case 145:
#line 1542 "../src/parser/XQParser.y"
    {
    (yyval.templateArg) = (yyvsp[(1) - (2)].templateArg);
    RESOLVE_QNAME((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].str));
    (yyval.templateArg)->uri = uri;
    (yyval.templateArg)->name = name;
  }
    break;

  case 146:
#line 1549 "../src/parser/XQParser.y"
    {
    (yyval.templateArg) = (yyvsp[(1) - (3)].templateArg);
    (yyval.templateArg)->value = PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode));
  }
    break;

  case 147:
#line 1554 "../src/parser/XQParser.y"
    {
    (yyval.templateArg) = (yyvsp[(1) - (3)].templateArg);
    (yyval.templateArg)->seqType = (yyvsp[(3) - (3)].sequenceType);
  }
    break;

  case 148:
#line 1559 "../src/parser/XQParser.y"
    {
    (yyval.templateArg) = (yyvsp[(1) - (2)].templateArg);
    // TBD tunnel parameters - jpcs
  }
    break;

  case 149:
#line 1567 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (2)].astNode);
  }
    break;

  case 150:
#line 1573 "../src/parser/XQParser.y"
    {
    // TBD xsl:fallback - jpcs
    (yyval.astNode) = PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode));
  }
    break;

  case 151:
#line 1581 "../src/parser/XQParser.y"
    {
    ASTNode *empty = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQSequence(MEMMGR));
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQIf((yyvsp[(1) - (3)].astNode), (yyvsp[(2) - (3)].parenExpr), empty, MEMMGR));
  }
    break;

  case 152:
#line 1589 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode));
  }
    break;

  case 153:
#line 1596 "../src/parser/XQParser.y"
    {
    XQIf *iff = (XQIf*)(yyvsp[(2) - (4)].astNode);
    while(iff->getWhenFalse() != 0) {
      iff = (XQIf*)iff->getWhenFalse();
    }

    iff->setWhenFalse((yyvsp[(3) - (4)].astNode));

    (yyval.astNode) = (yyvsp[(2) - (4)].astNode);
  }
    break;

  case 154:
#line 1610 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (1)].astNode);
  }
    break;

  case 155:
#line 1614 "../src/parser/XQParser.y"
    {
    XQIf *iff = (XQIf*)(yyvsp[(1) - (2)].astNode);
    while(iff->getWhenFalse() != 0) {
      iff = (XQIf*)iff->getWhenFalse();
    }

    iff->setWhenFalse((yyvsp[(2) - (2)].astNode));

    (yyval.astNode) = (yyvsp[(1) - (2)].astNode);
  }
    break;

  case 156:
#line 1628 "../src/parser/XQParser.y"
    {
    XQIf *iff = (XQIf*)(yyvsp[(1) - (3)].astNode);
    iff->setWhenTrue((yyvsp[(2) - (3)].parenExpr));
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
  }
    break;

  case 157:
#line 1637 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQIf(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)), 0, 0, MEMMGR));
  }
    break;

  case 158:
#line 1644 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yyloc), new (MEMMGR) XQSequence(MEMMGR));
  }
    break;

  case 159:
#line 1648 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(2) - (3)].parenExpr);
  }
    break;

  case 160:
#line 1655 "../src/parser/XQParser.y"
    {
    // TBD xsl:fallback - jpcs

    XQAnalyzeString *as = (XQAnalyzeString*)(yyval.astNode);
    (yyval.astNode) = (yyvsp[(1) - (4)].astNode);

    if(as->getExpression() == 0) {
      yyerror((yylsp[(1) - (4)]), "The xsl:analyze-string instruction does not have a {}select attribute");
    }

    if(as->getRegex() == 0) {
      yyerror((yylsp[(1) - (4)]), "The xsl:analyze-string instruction does not have a {}regex attribute");
    }

    if((yyvsp[(2) - (4)].astNode) == 0) {
      if((yyvsp[(3) - (4)].astNode) == 0) {
        yyerror((yylsp[(1) - (4)]), "The xsl:analyze-string instruction doesn't contain an xsl:matching-substring or xsl:non-matching-substring child [err:XTSE1130]");
      }

      as->setMatch(WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQSequence(MEMMGR)));
    }
    else {
      as->setMatch((yyvsp[(2) - (4)].astNode));
    }
    if((yyvsp[(3) - (4)].astNode) == 0) {
      as->setNonMatch(WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQSequence(MEMMGR)));
    }
    else {
      as->setNonMatch((yyvsp[(3) - (4)].astNode));
    }
  }
    break;

  case 161:
#line 1690 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yyloc), new (MEMMGR) XQAnalyzeString(MEMMGR));
  }
    break;

  case 162:
#line 1694 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ((XQAnalyzeString*)(yyval.astNode))->setExpression(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));
  }
    break;

  case 163:
#line 1699 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ASTNode *content = WRAP((yylsp[(3) - (3)]), new (MEMMGR) XQSimpleContent((yyvsp[(3) - (3)].itemList), MEMMGR));
    ((XQAnalyzeString*)(yyval.astNode))->setRegex(PRESERVE_NS((yylsp[(2) - (3)]), content));
  }
    break;

  case 164:
#line 1705 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ASTNode *content = WRAP((yylsp[(3) - (3)]), new (MEMMGR) XQSimpleContent((yyvsp[(3) - (3)].itemList), MEMMGR));
    ((XQAnalyzeString*)(yyval.astNode))->setFlags(PRESERVE_NS((yylsp[(2) - (3)]), content));
  }
    break;

  case 165:
#line 1714 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = 0;
  }
    break;

  case 166:
#line 1718 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(2) - (3)].parenExpr);
  }
    break;

  case 167:
#line 1725 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = 0;
  }
    break;

  case 168:
#line 1729 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(2) - (3)].parenExpr);
  }
    break;

  case 169:
#line 1736 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (4)].letTuple)->setExpression(XSLT_VARIABLE_VALUE((yylsp[(1) - (4)]), (yyvsp[(1) - (4)].letTuple)->getExpression(), (yyvsp[(2) - (4)].parenExpr), (yyvsp[(1) - (4)].letTuple)->seqType));
    (yyvsp[(1) - (4)].letTuple)->seqType = 0;

    if((yyvsp[(1) - (4)].letTuple)->getExpression() == 0) {
      yyerror((yylsp[(1) - (4)]), "The xsl:variable instruction has both a select attribute and a sequence constructor [err:XTSE0620]");
    }

    if((yyvsp[(1) - (4)].letTuple)->getVarName() == 0) {
      yyerror((yylsp[(1) - (4)]), "The xsl:variable instruction does not have a {}name attribute");
    }

    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQReturn((yyvsp[(1) - (4)].letTuple), (yyvsp[(4) - (4)].parenExpr), MEMMGR));
  }
    break;

  case 170:
#line 1754 "../src/parser/XQParser.y"
    {
    (yyval.letTuple) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) LetTuple(WRAP((yylsp[(1) - (1)]), new (MEMMGR) ContextTuple(MEMMGR)), 0, 0, MEMMGR));
  }
    break;

  case 171:
#line 1758 "../src/parser/XQParser.y"
    {
    RESOLVE_QNAME((yylsp[(2) - (2)]), (yyvsp[(2) - (2)].str));
    (yyvsp[(1) - (2)].letTuple)->setVarURI(uri);
    (yyvsp[(1) - (2)].letTuple)->setVarName(name);
    (yyval.letTuple) = (yyvsp[(1) - (2)].letTuple);
  }
    break;

  case 172:
#line 1765 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].letTuple)->setExpression((yyvsp[(3) - (3)].astNode));
    (yyval.letTuple) = (yyvsp[(1) - (3)].letTuple);
  }
    break;

  case 173:
#line 1770 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].letTuple)->seqType = (yyvsp[(3) - (3)].sequenceType);
    (yyval.letTuple) = (yyvsp[(1) - (3)].letTuple);
  }
    break;

  case 174:
#line 1778 "../src/parser/XQParser.y"
    {
    XQCommentConstructor *comment = (XQCommentConstructor*)(yyvsp[(1) - (3)].astNode);
    (yyval.astNode) = comment;

    if(!(yyvsp[(2) - (3)].parenExpr)->getChildren().empty()) {
      if(comment->getValue() != 0) {
        yyerror((yylsp[(1) - (3)]), "The xsl:comment instruction has both a select attribute and a sequence constructor [err:XTSE0940]");
      }
      comment->setValue((yyvsp[(2) - (3)].parenExpr));
    }
    else if(comment->getValue() == 0) {
      comment->setValue(WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQSequence(MEMMGR)));
    }
  }
    break;

  case 175:
#line 1796 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQCommentConstructor(0, MEMMGR, /*xslt*/true));
  }
    break;

  case 176:
#line 1800 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ((XQCommentConstructor*)(yyval.astNode))->setValue(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));
  }
    break;

  case 177:
#line 1808 "../src/parser/XQParser.y"
    {
    XQPIConstructor *pi = (XQPIConstructor*)(yyvsp[(1) - (3)].astNode);
    (yyval.astNode) = pi;

    if(pi->getName() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:processing-instruction instruction does not have a {}name attribute");
    }

    if(!(yyvsp[(2) - (3)].parenExpr)->getChildren().empty()) {
      if(pi->getValue() != 0) {
        yyerror((yylsp[(1) - (3)]), "The xsl:processing-instruction instruction has both a select attribute and a sequence constructor [err:XTSE0880]");
      }
      pi->setValue((yyvsp[(2) - (3)].parenExpr));
    }
    else if(pi->getValue() == 0) {
      pi->setValue(WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQSequence(MEMMGR)));
    }
  }
    break;

  case 178:
#line 1830 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQPIConstructor(0, 0, MEMMGR, /*xslt*/true));
  }
    break;

  case 179:
#line 1834 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);

    ASTNode *content = WRAP((yylsp[(3) - (3)]), new (MEMMGR) XQSimpleContent((yyvsp[(3) - (3)].itemList), MEMMGR));
    ((XQPIConstructor*)(yyval.astNode))->setName(PRESERVE_NS((yylsp[(2) - (3)]), content));
  }
    break;

  case 180:
#line 1841 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ((XQPIConstructor*)(yyval.astNode))->setValue(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));
  }
    break;

  case 181:
#line 1849 "../src/parser/XQParser.y"
    {
    XQDocumentConstructor *doc = (XQDocumentConstructor*)(yyvsp[(1) - (3)].astNode);
    doc->setValue((yyvsp[(2) - (3)].parenExpr));
    (yyval.astNode) = doc;
  }
    break;

  case 182:
#line 1858 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQDocumentConstructor(0, MEMMGR));
  }
    break;

  case 183:
#line 1866 "../src/parser/XQParser.y"
    {
    XQAttributeConstructor *attr = (XQAttributeConstructor*)(yyvsp[(1) - (3)].astNode);
    (yyval.astNode) = attr;

    if(attr->getName() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:attribute instruction does not have a {}name attribute");
    }

    if(attr->namespaceExpr != 0) {
      // Use fn:QName() to assign the correct URI
      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      args.push_back(attr->namespaceExpr);
      args.push_back(const_cast<ASTNode*>(attr->getName()));
      FunctionQName *name = WRAP((yylsp[(1) - (3)]), new (MEMMGR) FunctionQName(args, MEMMGR));
      attr->setName(name);
      attr->namespaceExpr = 0;
    }

    if(!(yyvsp[(2) - (3)].parenExpr)->getChildren().empty()) {
      if(attr->getChildren() != 0 && !attr->getChildren()->empty()) {
        yyerror((yylsp[(1) - (3)]), "The xsl:attribute instruction has both a select attribute and a sequence constructor [err:XTSE0840]");
      }

      VectorOfASTNodes *children = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
      *children = (yyvsp[(2) - (3)].parenExpr)->getChildren();
      attr->setChildren(children);
    }
    else if(attr->getChildren() == 0) {
      VectorOfASTNodes *children = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
      children->push_back(WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQSequence(MEMMGR)));
      attr->setChildren(children);
    }
  }
    break;

  case 184:
#line 1903 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQAttributeConstructor(0, 0, MEMMGR));
  }
    break;

  case 185:
#line 1907 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);

    ASTNode *content = WRAP((yylsp[(3) - (3)]), new (MEMMGR) XQSimpleContent((yyvsp[(3) - (3)].itemList), MEMMGR));
    ((XQAttributeConstructor*)(yyval.astNode))->setName(PRESERVE_NS((yylsp[(2) - (3)]), content));
  }
    break;

  case 186:
#line 1914 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);

    ASTNode *content = WRAP((yylsp[(3) - (3)]), new (MEMMGR) XQSimpleContent((yyvsp[(3) - (3)].itemList), MEMMGR));
    ((XQAttributeConstructor*)(yyval.astNode))->namespaceExpr = PRESERVE_NS((yylsp[(2) - (3)]), content);
  }
    break;

  case 187:
#line 1921 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);

    VectorOfASTNodes *children = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    children->push_back(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));

    ((XQAttributeConstructor*)(yyval.astNode))->setChildren(children);
  }
    break;

  case 188:
#line 1930 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    // TBD separator - jpcs
/*     ((XQAttributeConstructor*)$$)->setChildren(children); */
  }
    break;

  case 189:
#line 1939 "../src/parser/XQParser.y"
    {
    XQNamespaceConstructor *ns = (XQNamespaceConstructor*)(yyvsp[(1) - (3)].astNode);
    (yyval.astNode) = ns;

    if(ns->getName() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:namespace instruction does not have a {}name attribute");
    }

    if(!(yyvsp[(2) - (3)].parenExpr)->getChildren().empty()) {
      if(ns->getChildren() != 0 && !ns->getChildren()->empty()) {
        yyerror((yylsp[(1) - (3)]), "The xsl:namespace instruction has both a select attribute and a sequence constructor [err:XTSE0840]");
      }

      VectorOfASTNodes *children = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
      *children = (yyvsp[(2) - (3)].parenExpr)->getChildren();
      ns->setChildren(children);
    }
    else if(ns->getChildren() == 0) {
      VectorOfASTNodes *children = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
      children->push_back(WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQSequence(MEMMGR)));
      ns->setChildren(children);
    }
  }
    break;

  case 190:
#line 1966 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQNamespaceConstructor(0, 0, MEMMGR));
  }
    break;

  case 191:
#line 1970 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);

    ASTNode *content = WRAP((yylsp[(3) - (3)]), new (MEMMGR) XQSimpleContent((yyvsp[(3) - (3)].itemList), MEMMGR));
    ((XQNamespaceConstructor*)(yyval.astNode))->setName(PRESERVE_NS((yylsp[(2) - (3)]), content));
  }
    break;

  case 192:
#line 1977 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);

    VectorOfASTNodes *children = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    children->push_back(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));

    ((XQNamespaceConstructor*)(yyval.astNode))->setChildren(children);
  }
    break;

  case 193:
#line 1989 "../src/parser/XQParser.y"
    {
    XQElementConstructor *elem = (XQElementConstructor*)(yyvsp[(1) - (3)].astNode);
    (yyval.astNode) = elem;

    if(elem->getName() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:element instruction does not have a {}name attribute");
    }

    if(elem->namespaceExpr != 0) {
      // Use fn:QName() to assign the correct URI
      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      args.push_back(elem->namespaceExpr);
      args.push_back(const_cast<ASTNode*>(elem->getName()));
      FunctionQName *name = WRAP((yylsp[(1) - (3)]), new (MEMMGR) FunctionQName(args, MEMMGR));
      elem->setName(name);
      elem->namespaceExpr = 0;
    }

    VectorOfASTNodes *children = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    *children = (yyvsp[(2) - (3)].parenExpr)->getChildren();
    elem->setChildren(children);
  }
    break;

  case 194:
#line 2015 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQElementConstructor(0, 0, 0, MEMMGR));
  }
    break;

  case 195:
#line 2019 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);

    ASTNode *content = WRAP((yylsp[(3) - (3)]), new (MEMMGR) XQSimpleContent((yyvsp[(3) - (3)].itemList), MEMMGR));
    ((XQElementConstructor*)(yyval.astNode))->setName(PRESERVE_NS((yylsp[(2) - (3)]), content));
  }
    break;

  case 196:
#line 2026 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);

    ASTNode *content = WRAP((yylsp[(3) - (3)]), new (MEMMGR) XQSimpleContent((yyvsp[(3) - (3)].itemList), MEMMGR));
    ((XQElementConstructor*)(yyval.astNode))->namespaceExpr = PRESERVE_NS((yylsp[(2) - (3)]), content);
  }
    break;

  case 197:
#line 2036 "../src/parser/XQParser.y"
    {
    XQCopyOf *as = (XQCopyOf*)(yyval.astNode);
    (yyval.astNode) = (yyvsp[(1) - (2)].astNode);

    if(as->getExpression() == 0) {
      yyerror((yylsp[(1) - (2)]), "The xsl:copy-of instruction does not have a {}select attribute");
    }
  }
    break;

  case 198:
#line 2048 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yyloc), new (MEMMGR) XQCopyOf(MEMMGR));
  }
    break;

  case 199:
#line 2052 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ((XQCopyOf*)(yyval.astNode))->setExpression(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));
  }
    break;

  case 200:
#line 2057 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (2)].astNode);
    ((XQCopyOf*)(yyval.astNode))->setCopyNamespaces((yyvsp[(2) - (2)].boolean));
  }
    break;

  case 201:
#line 2066 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    XQCopy *as = (XQCopy*)(yyval.astNode);

    XQContextItem *ci = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQContextItem(MEMMGR));
    as->setExpression(ci);

    as->setChildren((yyvsp[(2) - (3)].parenExpr)->getChildren());
  }
    break;

  case 202:
#line 2079 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yyloc), new (MEMMGR) XQCopy(MEMMGR));
  }
    break;

  case 203:
#line 2083 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (2)].astNode);
    ((XQCopy*)(yyval.astNode))->setCopyNamespaces((yyvsp[(2) - (2)].boolean));
  }
    break;

  case 204:
#line 2088 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (2)].astNode);
    ((XQCopy*)(yyval.astNode))->setInheritNamespaces((yyvsp[(2) - (2)].boolean));
  }
    break;

  case 205:
#line 2097 "../src/parser/XQParser.y"
    {
    // TBD xsl:sort - jpcs
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    XQMap *map = (XQMap*)(yyval.astNode);
    map->setArg2((yyvsp[(2) - (3)].parenExpr));

    if(map->getArg1() == 0) {
      yyerror((yylsp[(1) - (3)]), "The xsl:for-each instruction does not have a {}select attribute");
    }
  }
    break;

  case 206:
#line 2111 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQMap(0, 0, MEMMGR));
  }
    break;

  case 207:
#line 2115 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    ((XQMap*)(yyval.astNode))->setArg1(PRESERVE_NS((yylsp[(2) - (3)]), (yyvsp[(3) - (3)].astNode)));
  }
    break;

  case 208:
#line 2128 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (3)].itemList);
    (yyval.itemList)->push_back(PRESERVE_NS((yylsp[(3) - (3)]), (yyvsp[(3) - (3)].astNode)));
  }
    break;

  case 209:
#line 2133 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    (yyval.itemList)->push_back(PRESERVE_NS((yylsp[(1) - (1)]), (yyvsp[(1) - (1)].astNode)));
  }
    break;

  case 210:
#line 2145 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (1)].astNode);
  }
    break;

  case 211:
#line 2149 "../src/parser/XQParser.y"
    {
    NodeTest *nt = new (MEMMGR) NodeTest();
    nt->setNodeType(Node::document_string);
    nt->setNameWildcard();
    nt->setNamespaceWildcard();
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQStep(XQStep::SELF, nt, MEMMGR));
  }
    break;

  case 212:
#line 2157 "../src/parser/XQParser.y"
    {
    // . intersect id("a")

    VectorOfASTNodes oargs(XQillaAllocator<ASTNode*>(MEMMGR));
    oargs.push_back(WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQContextItem(MEMMGR)));
    oargs.push_back((yyvsp[(1) - (1)].astNode));

    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) Intersect(oargs, MEMMGR));
  }
    break;

  case 213:
#line 2170 "../src/parser/XQParser.y"
    {
    NodeTest *nt = new (MEMMGR) NodeTest();
    nt->setNodeType(Node::document_string);
    nt->setNameWildcard();
    nt->setNamespaceWildcard();
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQStep(XQStep::PARENT, nt, MEMMGR));
  }
    break;

  case 214:
#line 2178 "../src/parser/XQParser.y"
    {
    NodeTest *nt = new (MEMMGR) NodeTest();
    nt->setNodeType(Node::document_string);
    nt->setNameWildcard();
    nt->setNamespaceWildcard();
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQStep(XQStep::ANCESTOR, nt, MEMMGR));
  }
    break;

  case 215:
#line 2191 "../src/parser/XQParser.y"
    {
    VectorOfASTNodes fargs(XQillaAllocator<ASTNode*>(MEMMGR));
    fargs.push_back((yyvsp[(3) - (4)].astNode));

    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) FunctionId(fargs, MEMMGR));
  }
    break;

  case 216:
#line 2198 "../src/parser/XQParser.y"
    {
//     VectorOfASTNodes fargs(XQillaAllocator<ASTNode*>(MEMMGR));
//     fargs.push_back($3);
//     fargs.push_back($5);

//     $$ = WRAP(@1, new (MEMMGR) FunctionKey(fargs, MEMMGR));

    // TBD key() - jpcs
    (yyval.astNode) = WRAP((yylsp[(1) - (6)]), new (MEMMGR) XQContextItem(MEMMGR));
  }
    break;

  case 221:
#line 2237 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(1) - (1)].astNode);
  }
    break;

  case 222:
#line 2241 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQPredicate((yyvsp[(2) - (2)].astNode), (yyvsp[(1) - (2)].astNode), MEMMGR));
  }
    break;

  case 223:
#line 2245 "../src/parser/XQParser.y"
    {
    // id("a")/foo -> self::foo[parent::node() intersect id("a")]

    NodeTest *nt = new (MEMMGR) NodeTest();
    nt->setTypeWildcard();
    nt->setNameWildcard();
    nt->setNamespaceWildcard();
    XQStep *step = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQStep(XQStep::PARENT, nt, MEMMGR));

    VectorOfASTNodes oargs(XQillaAllocator<ASTNode*>(MEMMGR));
    oargs.push_back(step);
    oargs.push_back((yyvsp[(1) - (3)].astNode));

    Intersect *intersect = WRAP((yylsp[(1) - (3)]), new (MEMMGR) Intersect(oargs, MEMMGR));

    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQPredicate((yyvsp[(3) - (3)].astNode), intersect, MEMMGR));
  }
    break;

  case 224:
#line 2263 "../src/parser/XQParser.y"
    {
    // id("a")/foo -> self::foo[ancestor::node() intersect id("a")]

    NodeTest *nt = new (MEMMGR) NodeTest();
    nt->setTypeWildcard();
    nt->setNameWildcard();
    nt->setNamespaceWildcard();
    XQStep *step = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQStep(XQStep::ANCESTOR, nt, MEMMGR));

    VectorOfASTNodes oargs(XQillaAllocator<ASTNode*>(MEMMGR));
    oargs.push_back(step);
    oargs.push_back((yyvsp[(1) - (3)].astNode));

    Intersect *intersect = WRAP((yylsp[(1) - (3)]), new (MEMMGR) Intersect(oargs, MEMMGR));

    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQPredicate((yyvsp[(3) - (3)].astNode), intersect, MEMMGR));
  }
    break;

  case 225:
#line 2281 "../src/parser/XQParser.y"
    {
    ASTNode *step = (yyvsp[(1) - (3)].astNode);
    while(step->getType() == ASTNode::PREDICATE)
      step = (ASTNode*)((XQPredicate*)step)->getExpression();

    ((XQStep*)step)->setAxis(XQStep::PARENT);

    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) XQPredicate((yyvsp[(3) - (3)].astNode), (yyvsp[(1) - (3)].astNode), MEMMGR));
  }
    break;

  case 226:
#line 2291 "../src/parser/XQParser.y"
    {
    ASTNode *step = (yyvsp[(1) - (3)].astNode);
    while(step->getType() == ASTNode::PREDICATE)
      step = (ASTNode*)((XQPredicate*)step)->getExpression();

    ((XQStep*)step)->setAxis(XQStep::ANCESTOR);

    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) XQPredicate((yyvsp[(3) - (3)].astNode), (yyvsp[(1) - (3)].astNode), MEMMGR));
  }
    break;

  case 227:
#line 2305 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = XQPredicate::addPredicates(WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQStep(XQStep::SELF, (yyvsp[(1) - (2)].nodeTest), MEMMGR)), (yyvsp[(2) - (2)].predicates));
  }
    break;

  case 228:
#line 2313 "../src/parser/XQParser.y"
    {
    if(!(yyvsp[(1) - (1)].nodeTest)->isNodeTypeSet())
      (yyvsp[(1) - (1)].nodeTest)->setNodeType(Node::element_string);
    (yyval.nodeTest) = (yyvsp[(1) - (1)].nodeTest);
  }
    break;

  case 229:
#line 2319 "../src/parser/XQParser.y"
    {
    if(!(yyvsp[(3) - (3)].nodeTest)->isNodeTypeSet())
      (yyvsp[(3) - (3)].nodeTest)->setNodeType(Node::element_string);
    (yyval.nodeTest) = (yyvsp[(3) - (3)].nodeTest);
  }
    break;

  case 230:
#line 2325 "../src/parser/XQParser.y"
    {
    if(!(yyvsp[(3) - (3)].nodeTest)->isNodeTypeSet())
      (yyvsp[(3) - (3)].nodeTest)->setNodeType(Node::attribute_string);
    (yyval.nodeTest) = (yyvsp[(3) - (3)].nodeTest);
  }
    break;

  case 231:
#line 2331 "../src/parser/XQParser.y"
    {
    if(!(yyvsp[(2) - (2)].nodeTest)->isNodeTypeSet())
      (yyvsp[(2) - (2)].nodeTest)->setNodeType(Node::attribute_string);
    (yyval.nodeTest) = (yyvsp[(2) - (2)].nodeTest);
  }
    break;

  case 232:
#line 2342 "../src/parser/XQParser.y"
    {
    (yyval.predicates) = new (MEMMGR) VectorOfPredicates(MEMMGR);
  }
    break;

  case 233:
#line 2346 "../src/parser/XQParser.y"
    {
    XQPredicate *pred = WRAP((yylsp[(2) - (4)]), new (MEMMGR) XQPredicate((yyvsp[(3) - (4)].astNode), MEMMGR));
    (yyvsp[(1) - (4)].predicates)->push_back(pred);
    (yyval.predicates) = (yyvsp[(1) - (4)].predicates); 
  }
    break;

  case 241:
#line 2376 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a setter");
  }
    break;

  case 242:
#line 2380 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "an import");
  }
    break;

  case 243:
#line 2384 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a namespace");
  }
    break;

  case 244:
#line 2388 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a default namespace");
  }
    break;

  case 245:
#line 2392 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 246:
#line 2396 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    XQUserFunction* decl=(yyvsp[(2) - (3)].functDecl);
    QP->_query->addFunction(decl);
  }
    break;

  case 247:
#line 2402 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    XQUserFunction* decl=(yyvsp[(2) - (3)].functDecl);
    QP->_query->addFunction(decl);
  }
    break;

  case 248:
#line 2408 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 256:
#line 2434 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) XQUserFunction((yyvsp[(3) - (5)].str), (yyvsp[(4) - (5)].argSpecs), (yyvsp[(5) - (5)].astNode), NULL, false, true, MEMMGR));
  }
    break;

  case 257:
#line 2438 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (7)]), new (MEMMGR) XQUserFunction((yyvsp[(3) - (7)].str), (yyvsp[(4) - (7)].argSpecs), (yyvsp[(7) - (7)].astNode), (yyvsp[(6) - (7)].sequenceType), false, true, MEMMGR));
  }
    break;

  case 258:
#line 2442 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) XQUserFunction((yyvsp[(3) - (5)].str), (yyvsp[(4) - (5)].argSpecs), NULL, NULL, false, true, MEMMGR));
  }
    break;

  case 259:
#line 2446 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (7)]), new (MEMMGR) XQUserFunction((yyvsp[(3) - (7)].str), (yyvsp[(4) - (7)].argSpecs), NULL, (yyvsp[(6) - (7)].sequenceType), false, true, MEMMGR));
  }
    break;

  case 269:
#line 2475 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a setter");
  }
    break;

  case 270:
#line 2479 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "an import");
  }
    break;

  case 271:
#line 2483 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a namespace");
  }
    break;

  case 272:
#line 2487 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a default namespace");
  }
    break;

  case 273:
#line 2491 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 274:
#line 2495 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    XQUserFunction* decl=(yyvsp[(2) - (3)].functDecl);
    QP->_query->addFunction(decl);
  }
    break;

  case 275:
#line 2501 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    XQUserFunction* decl=(yyvsp[(2) - (3)].functDecl);
    QP->_query->addFunction(decl);
  }
    break;

  case 276:
#line 2507 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 277:
#line 2511 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 285:
#line 2538 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a setter");
  }
    break;

  case 286:
#line 2542 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "an import");
  }
    break;

  case 287:
#line 2546 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a namespace");
  }
    break;

  case 288:
#line 2550 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a default namespace");
  }
    break;

  case 289:
#line 2554 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 290:
#line 2558 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    XQUserFunction* decl=(yyvsp[(2) - (3)].functDecl);
    QP->_query->addFunction(decl);
  }
    break;

  case 291:
#line 2564 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    XQUserFunction* decl=(yyvsp[(2) - (3)].functDecl);
    QP->_query->addFunction(decl);
  }
    break;

  case 292:
#line 2570 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 297:
#line 2589 "../src/parser/XQParser.y"
    {
    if(!XPath2Utils::equals((yyvsp[(3) - (4)].str),sz1_0))
      yyerror((yylsp[(2) - (4)]), "This XQuery processor only supports version 1.0 of the specs [err:XQST0031]");
  }
    break;

  case 298:
#line 2594 "../src/parser/XQParser.y"
    {
    if(!XPath2Utils::equals((yyvsp[(3) - (6)].str), sz1_0))
      yyerror((yylsp[(2) - (6)]), "This XQuery processor only supports version 1.0 of the specs [err:XQST0031]");

    XMLCh *encName = (yyvsp[(5) - (6)].str);
    if((*encName < chLatin_A || *encName > chLatin_Z) && (*encName < chLatin_a || *encName > chLatin_z))
      yyerror((yylsp[(5) - (6)]), "The specified encoding does not conform to the definition of EncName [err:XQST0087]");

    for(++encName; *encName; ++encName) {
      if((*encName < chLatin_A || *encName > chLatin_Z) &&
         (*encName < chLatin_a || *encName > chLatin_z) &&
         (*encName < chDigit_0 || *encName > chDigit_9) &&
         *encName != chPeriod && *encName != chDash)
        yyerror((yylsp[(5) - (6)]), "The specified encoding does not conform to the definition of EncName [err:XQST0087]");
    }

    // TODO: store the encoding somewhere
  }
    break;

  case 301:
#line 2623 "../src/parser/XQParser.y"
    {
    if(*(yyvsp[(5) - (6)].str) == 0)
      yyerror((yylsp[(5) - (6)]), "The literal that specifies the namespace of a module must not be of zero length [err:XQST0088]");
    QP->_query->setIsLibraryModule();
    QP->_query->setModuleTargetNamespace((yyvsp[(5) - (6)].str));
    SET_NAMESPACE((yylsp[(3) - (6)]), (yyvsp[(3) - (6)].str), (yyvsp[(5) - (6)].str));
  }
    break;

  case 303:
#line 2637 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a setter");
  }
    break;

  case 304:
#line 2641 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "an import");
  }
    break;

  case 305:
#line 2645 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a namespace");
  }
    break;

  case 306:
#line 2649 "../src/parser/XQParser.y"
    {
    CHECK_SECOND_STEP((yylsp[(2) - (3)]), "a default namespace");
  }
    break;

  case 307:
#line 2653 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 308:
#line 2657 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    XQUserFunction* decl=(yyvsp[(2) - (3)].functDecl);
    QP->_query->addFunction(decl);
  }
    break;

  case 309:
#line 2663 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
    XQUserFunction* decl=(yyvsp[(2) - (3)].functDecl);
    QP->_query->addFunction(decl);
  }
    break;

  case 310:
#line 2669 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 311:
#line 2673 "../src/parser/XQParser.y"
    {
    QP->_flags.set(BIT_DECLARE_SECOND_STEP);
  }
    break;

  case 323:
#line 2705 "../src/parser/XQParser.y"
    {
    SET_NAMESPACE((yylsp[(3) - (5)]), (yyvsp[(3) - (5)].str), (yyvsp[(5) - (5)].str));
  }
    break;

  case 324:
#line 2713 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_BOUNDARY_SPECIFIED, "boundary space", "XQST0068");
    CONTEXT->setPreserveBoundarySpace(true);
  }
    break;

  case 325:
#line 2718 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_BOUNDARY_SPECIFIED, "boundary space", "XQST0068");
    CONTEXT->setPreserveBoundarySpace(false);
  }
    break;

  case 326:
#line 2727 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (5)]), BIT_DEFAULTELEMENTNAMESPACE_SPECIFIED, "default element namespace", "XQST0066");
    CONTEXT->setDefaultElementAndTypeNS((yyvsp[(5) - (5)].str));
  }
    break;

  case 327:
#line 2732 "../src/parser/XQParser.y"
    { 
    CHECK_SPECIFIED((yylsp[(1) - (5)]), BIT_DEFAULTFUNCTIONNAMESPACE_SPECIFIED, "default function namespace", "XQST0066");
    CONTEXT->setDefaultFuncNS((yyvsp[(5) - (5)].str));
  }
    break;

  case 328:
#line 2741 "../src/parser/XQParser.y"
    {
    // validate the QName
    QualifiedName qName((yyvsp[(3) - (4)].str));
    const XMLCh* prefix = qName.getPrefix();
    if(prefix == 0 || *prefix == 0)
      yyerror((yylsp[(3) - (4)]), "The option name must have a prefix [err:XPST0081]");

    const XMLCh *uri = 0;
    try {
      LOCATION((yylsp[(3) - (4)]), loc);
      uri = CONTEXT->getUriBoundToPrefix(prefix, &loc);
    }
    catch(NamespaceLookupException&) {
      yyerror((yylsp[(3) - (4)]), "The option name is using an undefined namespace prefix [err:XPST0081]");
    }

    if(XPath2Utils::equals(uri, XQillaFunction::XMLChFunctionURI)) {
      if(XPath2Utils::equals(qName.getName(), option_projection)) {
        if(XPath2Utils::equals((yyvsp[(4) - (4)].str), SchemaSymbols::fgATTVAL_TRUE)) {
          CONTEXT->setProjection(true);
        }
        else if(XPath2Utils::equals((yyvsp[(4) - (4)].str), SchemaSymbols::fgATTVAL_FALSE)) {
          CONTEXT->setProjection(false);
        }
        else {
          yyerror((yylsp[(4) - (4)]), "Unknown value for option xqilla:projection. Should be 'true' or 'false' [err:XQILLA]");
        }
      }
      else if(XPath2Utils::equals(qName.getName(), option_psvi)) {
        if(XPath2Utils::equals((yyvsp[(4) - (4)].str), SchemaSymbols::fgATTVAL_TRUE)) {
          CONTEXT->getDocumentCache()->setDoPSVI(true);
        }
        else if(XPath2Utils::equals((yyvsp[(4) - (4)].str), SchemaSymbols::fgATTVAL_FALSE)) {
          CONTEXT->getDocumentCache()->setDoPSVI(false);
        }
        else {
          yyerror((yylsp[(4) - (4)]), "Unknown value for option xqilla:psvi. Should be 'true' or 'false' [err:XQILLA]");
        }
      }
      else if(XPath2Utils::equals(qName.getName(), option_lint)) {
        if(XPath2Utils::equals((yyvsp[(4) - (4)].str), SchemaSymbols::fgATTVAL_TRUE)) {
          CONTEXT->setDoLintWarnings(true);
        }
        else if(XPath2Utils::equals((yyvsp[(4) - (4)].str), SchemaSymbols::fgATTVAL_FALSE)) {
          CONTEXT->setDoLintWarnings(false);
        }
        else {
          yyerror((yylsp[(4) - (4)]), "Unknown value for option xqilla:lint. Should be 'true' or 'false' [err:XQILLA]");
        }
      }
      else {
        yyerror((yylsp[(3) - (4)]), "Unknown option name in the xqilla namespace [err:XQILLA]");
      }
    }
  }
    break;

  case 329:
#line 2801 "../src/parser/XQParser.y"
    {
  }
    break;

  case 330:
#line 2808 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_ORDERING_SPECIFIED, "ordering mode", "XQST0065");
    CONTEXT->setNodeSetOrdering(StaticContext::ORDERING_ORDERED);
  }
    break;

  case 331:
#line 2813 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_ORDERING_SPECIFIED, "ordering mode", "XQST0065");
    CONTEXT->setNodeSetOrdering(StaticContext::ORDERING_UNORDERED);
  }
    break;

  case 332:
#line 2822 "../src/parser/XQParser.y"
    { 
    CHECK_SPECIFIED((yylsp[(1) - (5)]), BIT_EMPTYORDERING_SPECIFIED, "empty ordering mode", "XQST0069");
    CONTEXT->setDefaultFLWOROrderingMode(StaticContext::FLWOR_ORDER_EMPTY_GREATEST);
  }
    break;

  case 333:
#line 2827 "../src/parser/XQParser.y"
    { 
    CHECK_SPECIFIED((yylsp[(1) - (5)]), BIT_EMPTYORDERING_SPECIFIED, "empty ordering mode", "XQST0069");
    CONTEXT->setDefaultFLWOROrderingMode(StaticContext::FLWOR_ORDER_EMPTY_LEAST);
  }
    break;

  case 334:
#line 2836 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (5)]), BIT_COPYNAMESPACE_SPECIFIED, "copy namespace", "XQST0055");
    CONTEXT->setPreserveNamespaces((yyvsp[(3) - (5)].boolean));
    CONTEXT->setInheritNamespaces((yyvsp[(5) - (5)].boolean));
  }
    break;

  case 335:
#line 2846 "../src/parser/XQParser.y"
    {
    (yyval.boolean) = true;
  }
    break;

  case 336:
#line 2850 "../src/parser/XQParser.y"
    {
    (yyval.boolean) = false;
  }
    break;

  case 337:
#line 2858 "../src/parser/XQParser.y"
    {
    (yyval.boolean) = true;
  }
    break;

  case 338:
#line 2862 "../src/parser/XQParser.y"
    {
    (yyval.boolean) = false;
  }
    break;

  case 339:
#line 2870 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (4)]), BIT_COLLATION_SPECIFIED, "default collation", "XQST0038");
    try {
      LOCATION((yylsp[(4) - (4)]), loc);
      CONTEXT->getCollation((yyvsp[(4) - (4)].str), &loc);
    }
    catch(ContextException&) {
      yyerror((yylsp[(4) - (4)]), "The specified collation does not exist [err:XQST0038]");
    }
    CONTEXT->setDefaultCollation((yyvsp[(4) - (4)].str));
  }
    break;

  case 340:
#line 2886 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_BASEURI_SPECIFIED, "base URI", "XQST0032");
    CONTEXT->setBaseURI((yyvsp[(3) - (3)].str));
  }
    break;

  case 341:
#line 2895 "../src/parser/XQParser.y"
    {
    if(XPath2Utils::equals((yyvsp[(3) - (5)].str), XMLUni::fgZeroLenString))
      CONTEXT->setDefaultElementAndTypeNS((yyvsp[(4) - (5)].str));
    else if(XPath2Utils::equals((yyvsp[(4) - (5)].str), XMLUni::fgZeroLenString))
      yyerror((yylsp[(1) - (5)]), "A schema that has no target namespace cannot be bound to a non-empty prefix [err:XQST0057]");
    else {
      SET_NAMESPACE((yylsp[(3) - (5)]), (yyvsp[(3) - (5)].str), (yyvsp[(4) - (5)].str));
    }
    LOCATION((yylsp[(1) - (5)]), loc);
    CONTEXT->addSchemaLocation((yyvsp[(4) - (5)].str), (yyvsp[(5) - (5)].stringList), &loc);
  }
    break;

  case 342:
#line 2907 "../src/parser/XQParser.y"
    {
    LOCATION((yylsp[(1) - (4)]), loc);
    CONTEXT->addSchemaLocation((yyvsp[(3) - (4)].str), (yyvsp[(4) - (4)].stringList), &loc);
  }
    break;

  case 343:
#line 2915 "../src/parser/XQParser.y"
    {
    (yyval.stringList) = NULL;
  }
    break;

  case 344:
#line 2919 "../src/parser/XQParser.y"
    {
    (yyval.stringList) = new (MEMMGR) VectorOfStrings(XQillaAllocator<const XMLCh*>(MEMMGR));
    (yyval.stringList)->push_back((yyvsp[(2) - (2)].str));
  }
    break;

  case 345:
#line 2924 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].stringList)->push_back((yyvsp[(3) - (3)].str));
    (yyval.stringList) = (yyvsp[(1) - (3)].stringList);
  }
    break;

  case 346:
#line 2933 "../src/parser/XQParser.y"
    {
    (yyval.str) = (yyvsp[(2) - (3)].str);
  }
    break;

  case 347:
#line 2937 "../src/parser/XQParser.y"
    {
    (yyval.str) = (XMLCh*)XMLUni::fgZeroLenString;
  }
    break;

  case 348:
#line 2945 "../src/parser/XQParser.y"
    {
    if(XMLString::stringLen((yyvsp[(6) - (7)].str))==0)
      yyerror((yylsp[(6) - (7)]), "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");

    SET_NAMESPACE((yylsp[(4) - (7)]), (yyvsp[(4) - (7)].str), (yyvsp[(6) - (7)].str));

    LOCATION((yylsp[(1) - (7)]), loc);
    QP->_query->importModule((yyvsp[(6) - (7)].str), (yyvsp[(7) - (7)].stringList), CONTEXT, &loc);
  }
    break;

  case 349:
#line 2955 "../src/parser/XQParser.y"
    {
    if(XMLString::stringLen((yyvsp[(3) - (4)].str))==0)
      yyerror((yylsp[(3) - (4)]), "The literal that specifies the target namespace in a module import must not be of zero length [err:XQST0088]");

    LOCATION((yylsp[(1) - (4)]), loc);
    QP->_query->importModule((yyvsp[(3) - (4)].str), (yyvsp[(4) - (4)].stringList), CONTEXT, &loc);
  }
    break;

  case 350:
#line 2967 "../src/parser/XQParser.y"
    {
    QP->_query->addVariable(WRAP((yylsp[(1) - (6)]), new (MEMMGR) XQGlobalVariable((yyvsp[(4) - (6)].str), (yyvsp[(5) - (6)].sequenceType), (yyvsp[(6) - (6)].astNode), MEMMGR)));
  }
    break;

  case 351:
#line 2974 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(2) - (2)].astNode);
  }
    break;

  case 352:
#line 2978 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = NULL;
  }
    break;

  case 353:
#line 2986 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_CONSTRUCTION_SPECIFIED, "construction mode", "XQST0067");
    CONTEXT->setConstructionMode(StaticContext::CONSTRUCTION_MODE_PRESERVE);
  }
    break;

  case 354:
#line 2991 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_CONSTRUCTION_SPECIFIED, "construction mode", "XQST0067");
    CONTEXT->setConstructionMode(StaticContext::CONSTRUCTION_MODE_STRIP);
  }
    break;

  case 355:
#line 3001 "../src/parser/XQParser.y"
    {
      (yyval.functDecl) = WRAP((yylsp[(1) - (6)]), new (MEMMGR) XQUserFunction((yyvsp[(4) - (6)].str),(yyvsp[(5) - (6)].argSpecs),WRAP((yylsp[(6) - (6)]), (yyvsp[(6) - (6)].astNode)),NULL, (yyvsp[(2) - (6)].boolean), true, MEMMGR));
    }
    break;

  case 356:
#line 3005 "../src/parser/XQParser.y"
    {
      (yyval.functDecl) = WRAP((yylsp[(1) - (8)]), new (MEMMGR) XQUserFunction((yyvsp[(4) - (8)].str),(yyvsp[(5) - (8)].argSpecs),WRAP((yylsp[(8) - (8)]), (yyvsp[(8) - (8)].astNode)),(yyvsp[(7) - (8)].sequenceType), (yyvsp[(2) - (8)].boolean), true, MEMMGR));
    }
    break;

  case 357:
#line 3009 "../src/parser/XQParser.y"
    {
      (yyval.functDecl) = WRAP((yylsp[(1) - (6)]), new (MEMMGR) XQUserFunction((yyvsp[(4) - (6)].str),(yyvsp[(5) - (6)].argSpecs),NULL,NULL, (yyvsp[(2) - (6)].boolean), true, MEMMGR));
    }
    break;

  case 358:
#line 3013 "../src/parser/XQParser.y"
    {
      (yyval.functDecl) = WRAP((yylsp[(1) - (8)]), new (MEMMGR) XQUserFunction((yyvsp[(4) - (8)].str),(yyvsp[(5) - (8)].argSpecs),NULL,(yyvsp[(7) - (8)].sequenceType), (yyvsp[(2) - (8)].boolean), true, MEMMGR));
    }
    break;

  case 359:
#line 3020 "../src/parser/XQParser.y"
    {
    (yyval.argSpecs) = NULL;
  }
    break;

  case 360:
#line 3024 "../src/parser/XQParser.y"
    {
    (yyval.argSpecs) = (yyvsp[(2) - (3)].argSpecs);
  }
    break;

  case 361:
#line 3030 "../src/parser/XQParser.y"
    {
    (yyval.boolean) = false;
  }
    break;

  case 362:
#line 3034 "../src/parser/XQParser.y"
    {
    (yyval.boolean) = true;
  }
    break;

  case 363:
#line 3042 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].argSpecs)->push_back((yyvsp[(3) - (3)].argSpec));
    (yyval.argSpecs) = (yyvsp[(1) - (3)].argSpecs);
  }
    break;

  case 364:
#line 3047 "../src/parser/XQParser.y"
    {
    XQUserFunction::ArgumentSpecs* paramList = new (MEMMGR) XQUserFunction::ArgumentSpecs(XQillaAllocator<XQUserFunction::ArgumentSpec*>(MEMMGR));
    paramList->push_back((yyvsp[(1) - (1)].argSpec));
    (yyval.argSpecs) = paramList;
  }
    break;

  case 365:
#line 3057 "../src/parser/XQParser.y"
    {
    (yyval.argSpec) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQUserFunction::ArgumentSpec((yyvsp[(2) - (3)].str), (yyvsp[(3) - (3)].sequenceType), MEMMGR));
  }
    break;

  case 366:
#line 3065 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = (yyvsp[(2) - (3)].astNode);
    }
    break;

  case 367:
#line 3069 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = (yyvsp[(2) - (3)].astNode);
    }
    break;

  case 368:
#line 3077 "../src/parser/XQParser.y"
    {
    if(QP->_lexer->isUpdate()) {
      QP->_query->setQueryBody(WRAP((yylsp[(1) - (1)]), new (MEMMGR) UApplyUpdates((yyvsp[(1) - (1)].astNode), MEMMGR)));
    }
    else {
      QP->_query->setQueryBody((yyvsp[(1) - (1)].astNode));
    }
  }
    break;

  case 369:
#line 3090 "../src/parser/XQParser.y"
    {
      ASTNode* prevExpr=(yyvsp[(1) - (3)].astNode);
      if(prevExpr->getType()==ASTNode::SEQUENCE)
      {
        ((XQSequence *)prevExpr)->addItem((yyvsp[(3) - (3)].astNode));
        (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
      }
      else {
        XQSequence *dis = WRAP((yylsp[(2) - (3)]), new (MEMMGR) XQSequence(MEMMGR));
        dis->addItem((yyvsp[(1) - (3)].astNode));
        dis->addItem((yyvsp[(3) - (3)].astNode));
        (yyval.astNode) = dis;
      }
    }
    break;

  case 383:
#line 3128 "../src/parser/XQParser.y"
    {
      // Add a ContextTuple at the start
      TupleNode *tmp = setLastAncestor((yyvsp[(1) - (3)].tupleNode), WRAP((yylsp[(1) - (3)]), new (MEMMGR) ContextTuple(MEMMGR)));

      // Add the return expression
      (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) XQReturn(tmp, (yyvsp[(3) - (3)].astNode), MEMMGR));
    }
    break;

  case 384:
#line 3139 "../src/parser/XQParser.y"
    {
      (yyval.tupleNode) = setLastAncestor((yyvsp[(3) - (3)].tupleNode), setLastAncestor((yyvsp[(2) - (3)].tupleNode), (yyvsp[(1) - (3)].tupleNode)));
    }
    break;

  case 385:
#line 3143 "../src/parser/XQParser.y"
    {
      (yyval.tupleNode) = setLastAncestor((yyvsp[(2) - (2)].tupleNode), (yyvsp[(1) - (2)].tupleNode));
    }
    break;

  case 386:
#line 3147 "../src/parser/XQParser.y"
    {
      (yyval.tupleNode) = setLastAncestor((yyvsp[(2) - (2)].tupleNode), (yyvsp[(1) - (2)].tupleNode));
    }
    break;

  case 388:
#line 3155 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FlworExprForLetList, (yylsp[(2) - (2)]));

      (yyval.tupleNode) = setLastAncestor((yyvsp[(2) - (2)].tupleNode), (yyvsp[(1) - (2)].tupleNode));
    }
    break;

  case 389:
#line 3161 "../src/parser/XQParser.y"
    {
      REJECT_NOT_XQUERY(FlworExprForLetList, (yylsp[(2) - (2)]));

      (yyval.tupleNode) = setLastAncestor((yyvsp[(2) - (2)].tupleNode), (yyvsp[(1) - (2)].tupleNode));
    }
    break;

  case 392:
#line 3174 "../src/parser/XQParser.y"
    {
      (yyval.tupleNode) = (yyvsp[(2) - (2)].tupleNode);
    }
    break;

  case 393:
#line 3181 "../src/parser/XQParser.y"
    {
      (yyval.tupleNode) = setLastAncestor((yyvsp[(3) - (3)].tupleNode), (yyvsp[(1) - (3)].tupleNode));
    }
    break;

  case 395:
#line 3189 "../src/parser/XQParser.y"
    {
    // the SequenceType has been specified for each item of the sequence, but we can only apply to the
    // sequence itself, so allow it to match multiple matches
    (yyvsp[(3) - (7)].sequenceType)->setOccurrence(SequenceType::STAR);
    (yyval.tupleNode) = WRAP((yylsp[(1) - (7)]), new (MEMMGR) ForTuple(0, (yyvsp[(2) - (7)].str), (yyvsp[(4) - (7)].str), WRAP((yylsp[(3) - (7)]), new (MEMMGR) XQTreatAs((yyvsp[(7) - (7)].astNode), (yyvsp[(3) - (7)].sequenceType), MEMMGR)), MEMMGR));
  }
    break;

  case 396:
#line 3200 "../src/parser/XQParser.y"
    {
    (yyval.str) = NULL;
  }
    break;

  case 397:
#line 3204 "../src/parser/XQParser.y"
    { 
    REJECT_NOT_XQUERY(PositionalVar, (yylsp[(1) - (3)]));

    (yyval.str) = (yyvsp[(3) - (3)].str); 
  }
    break;

  case 398:
#line 3214 "../src/parser/XQParser.y"
    {
    (yyval.str) = NULL;
  }
    break;

  case 399:
#line 3218 "../src/parser/XQParser.y"
    {
    (yyval.str) = (yyvsp[(3) - (3)].str);
  }
    break;

  case 400:
#line 3228 "../src/parser/XQParser.y"
    {
    (yyval.tupleNode) = (yyvsp[(2) - (2)].tupleNode);
  }
    break;

  case 401:
#line 3235 "../src/parser/XQParser.y"
    {
    (yyval.tupleNode) = setLastAncestor((yyvsp[(3) - (3)].tupleNode), (yyvsp[(1) - (3)].tupleNode));
  }
    break;

  case 403:
#line 3243 "../src/parser/XQParser.y"
    {
    (yyval.tupleNode) = WRAP((yylsp[(1) - (6)]), new (MEMMGR) LetTuple(0, (yyvsp[(2) - (6)].str), WRAP((yylsp[(3) - (6)]), new (MEMMGR) XQTreatAs((yyvsp[(6) - (6)].astNode), (yyvsp[(3) - (6)].sequenceType), MEMMGR)), MEMMGR));
  }
    break;

  case 404:
#line 3247 "../src/parser/XQParser.y"
    {
  }
    break;

  case 405:
#line 3254 "../src/parser/XQParser.y"
    { 
    (yyval.tupleNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) WhereTuple(0, (yyvsp[(2) - (2)].astNode), MEMMGR));
  }
    break;

  case 406:
#line 3262 "../src/parser/XQParser.y"
    {
    ((OrderByTuple*)getLastAncestor((yyvsp[(3) - (3)].tupleNode)))->setUnstable();
    (yyval.tupleNode) = (yyvsp[(3) - (3)].tupleNode);
  }
    break;

  case 407:
#line 3267 "../src/parser/XQParser.y"
    {
    (yyval.tupleNode) = (yyvsp[(4) - (4)].tupleNode);
  }
    break;

  case 408:
#line 3275 "../src/parser/XQParser.y"
    {
    (yyval.tupleNode) = setLastAncestor((yyvsp[(1) - (3)].tupleNode), (yyvsp[(3) - (3)].tupleNode));
  }
    break;

  case 410:
#line 3285 "../src/parser/XQParser.y"
    {
    LOCATION((yylsp[(1) - (3)]), loc);
    Collation *collation = CONTEXT->getDefaultCollation(&loc);
    
    (yyval.tupleNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) OrderByTuple(0, (yyvsp[(1) - (3)].astNode), OrderByTuple::STABLE | (yyvsp[(2) - (3)].orderByModifier) | (yyvsp[(3) - (3)].orderByModifier), collation, MEMMGR));
  }
    break;

  case 411:
#line 3292 "../src/parser/XQParser.y"
    {
    try {
      LOCATION((yylsp[(4) - (5)]), loc);
      Collation *collation = CONTEXT->getCollation((yyvsp[(5) - (5)].str), &loc);

      (yyval.tupleNode) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) OrderByTuple(0, (yyvsp[(1) - (5)].astNode), OrderByTuple::STABLE | (yyvsp[(2) - (5)].orderByModifier) | (yyvsp[(3) - (5)].orderByModifier), collation, MEMMGR));
    }
    catch(ContextException&) {
      yyerror((yylsp[(4) - (5)]), "The specified collation does not exist [err:XQST0076]");
    }
  }
    break;

  case 412:
#line 3307 "../src/parser/XQParser.y"
    {
    SequenceType *zero_or_one = WRAP((yylsp[(1) - (1)]), new (MEMMGR)
      SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING),
                   SequenceType::QUESTION_MARK));

    (yyval.astNode) = (yyvsp[(1) - (1)].astNode);
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQAtomize((yyval.astNode), MEMMGR));
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQPromoteUntyped((yyval.astNode), SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                                SchemaSymbols::fgDT_STRING, MEMMGR));
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQTreatAs((yyval.astNode), zero_or_one, MEMMGR));
  }
    break;

  case 413:
#line 3323 "../src/parser/XQParser.y"
    {
    (yyval.orderByModifier) = OrderByTuple::ASCENDING;
  }
    break;

  case 414:
#line 3327 "../src/parser/XQParser.y"
    {
    (yyval.orderByModifier) = OrderByTuple::ASCENDING;
  }
    break;

  case 415:
#line 3331 "../src/parser/XQParser.y"
    {
    (yyval.orderByModifier) = OrderByTuple::DESCENDING;
  }
    break;

  case 416:
#line 3338 "../src/parser/XQParser.y"
    {
    switch(CONTEXT->getDefaultFLWOROrderingMode()) {
    case StaticContext::FLWOR_ORDER_EMPTY_LEAST:
      (yyval.orderByModifier) = OrderByTuple::EMPTY_LEAST; break;
    case StaticContext::FLWOR_ORDER_EMPTY_GREATEST:
      (yyval.orderByModifier) = OrderByTuple::EMPTY_GREATEST; break;
    }
  }
    break;

  case 417:
#line 3347 "../src/parser/XQParser.y"
    {
    (yyval.orderByModifier) = OrderByTuple::EMPTY_GREATEST;
  }
    break;

  case 418:
#line 3351 "../src/parser/XQParser.y"
    {
    (yyval.orderByModifier) = OrderByTuple::EMPTY_LEAST;
  }
    break;

  case 419:
#line 3360 "../src/parser/XQParser.y"
    {
    // Add a ContextTuple at the start
    TupleNode *tmp = setLastAncestor((yyvsp[(2) - (4)].tupleNode), WRAP((yylsp[(1) - (4)]), new (MEMMGR) ContextTuple(MEMMGR)));

    // Add the return expression
    (yyval.astNode) = WRAP((yylsp[(3) - (4)]), new (MEMMGR) XQQuantified(XQQuantified::SOME, tmp, (yyvsp[(4) - (4)].astNode), MEMMGR));
  }
    break;

  case 420:
#line 3368 "../src/parser/XQParser.y"
    {
    // Add a ContextTuple at the start
    TupleNode *tmp = setLastAncestor((yyvsp[(2) - (4)].tupleNode), WRAP((yylsp[(1) - (4)]), new (MEMMGR) ContextTuple(MEMMGR)));

    // Add the return expression
    (yyval.astNode) = WRAP((yylsp[(3) - (4)]), new (MEMMGR) XQQuantified(XQQuantified::EVERY, tmp, (yyvsp[(4) - (4)].astNode), MEMMGR));
  }
    break;

  case 421:
#line 3379 "../src/parser/XQParser.y"
    {
    (yyval.tupleNode) = setLastAncestor((yyvsp[(3) - (3)].tupleNode), (yyvsp[(1) - (3)].tupleNode));
  }
    break;

  case 423:
#line 3387 "../src/parser/XQParser.y"
    {
    // the SequenceType has been specified for each item of the sequence, but we can only apply to the
    // sequence itself, so allow it to match multiple matches
    (yyvsp[(3) - (5)].sequenceType)->setOccurrence(SequenceType::STAR);
    (yyval.tupleNode) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) ForTuple(0, (yyvsp[(2) - (5)].str), 0, WRAP((yylsp[(3) - (5)]), new (MEMMGR) XQTreatAs((yyvsp[(5) - (5)].astNode), (yyvsp[(3) - (5)].sequenceType), MEMMGR)), MEMMGR));
  }
    break;

  case 424:
#line 3398 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = new (MEMMGR) XQTypeswitch( WRAP((yylsp[(1) - (6)]), (yyvsp[(3) - (6)].astNode)), (yyvsp[(5) - (6)].caseClauses), (yyvsp[(6) - (6)].caseClause), MEMMGR);
  }
    break;

  case 425:
#line 3405 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (2)].caseClauses)->push_back((yyvsp[(2) - (2)].caseClause));
    (yyval.caseClauses)=(yyvsp[(1) - (2)].caseClauses);
  }
    break;

  case 426:
#line 3410 "../src/parser/XQParser.y"
    {
    (yyval.caseClauses)=new (MEMMGR) XQTypeswitch::Cases(XQillaAllocator<XQTypeswitch::Case*>(MEMMGR));
    (yyval.caseClauses)->push_back((yyvsp[(1) - (1)].caseClause));
  }
    break;

  case 427:
#line 3418 "../src/parser/XQParser.y"
    {
    (yyval.caseClause) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) XQTypeswitch::Case((yyvsp[(3) - (5)].str), NULL, (yyvsp[(5) - (5)].astNode)));
  }
    break;

  case 428:
#line 3422 "../src/parser/XQParser.y"
    {
    (yyval.caseClause) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQTypeswitch::Case(NULL, NULL, (yyvsp[(3) - (3)].astNode)));
  }
    break;

  case 429:
#line 3430 "../src/parser/XQParser.y"
    { 
    (yyval.caseClause) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQTypeswitch::Case(NULL, (yyvsp[(2) - (4)].sequenceType), (yyvsp[(4) - (4)].astNode)));
  }
    break;

  case 430:
#line 3434 "../src/parser/XQParser.y"
    { 
    (yyval.caseClause) = WRAP((yylsp[(1) - (7)]), new (MEMMGR) XQTypeswitch::Case((yyvsp[(3) - (7)].str), (yyvsp[(5) - (7)].sequenceType), (yyvsp[(7) - (7)].astNode)));
  }
    break;

  case 431:
#line 3442 "../src/parser/XQParser.y"
    { 
    (yyval.astNode) = WRAP((yylsp[(1) - (8)]), new (MEMMGR) XQIf((yyvsp[(3) - (8)].astNode), (yyvsp[(6) - (8)].astNode), (yyvsp[(8) - (8)].astNode), MEMMGR));
  }
    break;

  case 432:
#line 3450 "../src/parser/XQParser.y"
    {
    if((yyvsp[(1) - (3)].astNode)->getType() == ASTNode::OPERATOR && ((XQOperator*)(yyvsp[(1) - (3)].astNode))->getOperatorName() == Or::name) {
      ((Or*)(yyvsp[(1) - (3)].astNode))->addArgument((yyvsp[(3) - (3)].astNode));
      (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    }
    else
      (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Or(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR), MEMMGR));
  }
    break;

  case 434:
#line 3464 "../src/parser/XQParser.y"
    {
    if((yyvsp[(1) - (3)].astNode)->getType()==ASTNode::OPERATOR && ((XQOperator*)(yyvsp[(1) - (3)].astNode))->getOperatorName() == And::name) {
      ((And*)(yyvsp[(1) - (3)].astNode))->addArgument((yyvsp[(3) - (3)].astNode));
      (yyval.astNode) = (yyvsp[(1) - (3)].astNode);
    }
    else
      (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) And(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR), MEMMGR));
  }
    break;

  case 436:
#line 3483 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) GeneralComp(GeneralComp::EQUAL,packageArgs((yyvsp[(1) - (3)].astNode),(yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 437:
#line 3487 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) GeneralComp(GeneralComp::NOT_EQUAL,packageArgs((yyvsp[(1) - (3)].astNode),(yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 438:
#line 3490 "../src/parser/XQParser.y"
    { /* Careful! */ QP->_lexer->undoLessThan(); }
    break;

  case 439:
#line 3491 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (4)]), new (MEMMGR) GeneralComp(GeneralComp::LESS_THAN,packageArgs((yyvsp[(1) - (4)].astNode),(yyvsp[(4) - (4)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 440:
#line 3495 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) GeneralComp(GeneralComp::LESS_THAN_EQUAL,packageArgs((yyvsp[(1) - (3)].astNode),(yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 441:
#line 3499 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) GeneralComp(GeneralComp::GREATER_THAN,packageArgs((yyvsp[(1) - (3)].astNode),(yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 442:
#line 3503 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) GeneralComp(GeneralComp::GREATER_THAN_EQUAL,packageArgs((yyvsp[(1) - (3)].astNode),(yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 443:
#line 3507 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Equals(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 444:
#line 3511 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) NotEquals(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 445:
#line 3515 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) LessThan(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 446:
#line 3519 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) LessThanEqual(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 447:
#line 3523 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) GreaterThan(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 448:
#line 3527 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) GreaterThanEqual(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 449:
#line 3531 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) NodeComparison(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR), MEMMGR));
  }
    break;

  case 450:
#line 3535 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) OrderComparison(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR), true, MEMMGR));
  }
    break;

  case 451:
#line 3539 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) OrderComparison(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR), false,MEMMGR));
  }
    break;

  case 453:
#line 3548 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) FTContains((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].ftselection), NULL, MEMMGR));
  }
    break;

  case 454:
#line 3552 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (4)]), new (MEMMGR) FTContains((yyvsp[(1) - (4)].astNode), (yyvsp[(3) - (4)].ftselection), (yyvsp[(4) - (4)].astNode), MEMMGR));
  }
    break;

  case 456:
#line 3561 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Range(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 458:
#line 3570 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Plus(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 459:
#line 3574 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Minus(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 461:
#line 3583 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Multiply(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 462:
#line 3587 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Divide(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 463:
#line 3591 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) IntegerDivide(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 464:
#line 3595 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Mod(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
  }
    break;

  case 466:
#line 3604 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Union(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
    }
    break;

  case 467:
#line 3608 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Union(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
    }
    break;

  case 469:
#line 3617 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Intersect(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
    }
    break;

  case 470:
#line 3621 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) Except(packageArgs((yyvsp[(1) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR),MEMMGR));
    }
    break;

  case 472:
#line 3630 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (4)]), new (MEMMGR) XQInstanceOf((yyvsp[(1) - (4)].astNode),(yyvsp[(4) - (4)].sequenceType),MEMMGR));
  }
    break;

  case 474:
#line 3639 "../src/parser/XQParser.y"
    {
    XQTreatAs* treatAs = new (MEMMGR) XQTreatAs((yyvsp[(1) - (4)].astNode),(yyvsp[(4) - (4)].sequenceType),MEMMGR, XQTreatAs::err_XPDY0050);
    (yyval.astNode) = WRAP((yylsp[(2) - (4)]), treatAs);
  }
    break;

  case 476:
#line 3649 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(2) - (4)]), new (MEMMGR) XQCastableAs((yyvsp[(1) - (4)].astNode),(yyvsp[(4) - (4)].sequenceType),MEMMGR));
  }
    break;

  case 478:
#line 3658 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQCastAs((yyvsp[(1) - (4)].astNode),(yyvsp[(4) - (4)].sequenceType),MEMMGR));
  }
    break;

  case 480:
#line 3667 "../src/parser/XQParser.y"
    {
      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      args.push_back((yyvsp[(2) - (2)].astNode));
      (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) UnaryMinus(/*positive*/false, args, MEMMGR));
    }
    break;

  case 481:
#line 3673 "../src/parser/XQParser.y"
    {
      VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
      args.push_back((yyvsp[(2) - (2)].astNode));
      (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) UnaryMinus(/*positive*/true, args, MEMMGR));
    }
    break;

  case 486:
#line 3694 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQValidate((yyvsp[(3) - (4)].astNode),DocumentCache::VALIDATION_STRICT,MEMMGR));
    }
    break;

  case 487:
#line 3698 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) XQValidate((yyvsp[(4) - (5)].astNode),DocumentCache::VALIDATION_LAX,MEMMGR));
    }
    break;

  case 488:
#line 3702 "../src/parser/XQParser.y"
    {
      (yyval.astNode) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) XQValidate((yyvsp[(4) - (5)].astNode),DocumentCache::VALIDATION_STRICT,MEMMGR));
    }
    break;

  case 489:
#line 3710 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ExtensionExpr, (yylsp[(1) - (3)]));

    // we don't support any pragma
    yyerror((yylsp[(1) - (3)]), "This pragma is not recognized, and no alternative expression is specified [err:XQST0079]");
  }
    break;

  case 490:
#line 3717 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(ExtensionExpr, (yylsp[(1) - (4)]));

    // we don't support any pragma
    (yyval.astNode) = (yyvsp[(3) - (4)].astNode);
  }
    break;

  case 493:
#line 3730 "../src/parser/XQParser.y"
    {
        // validate the QName
        QualifiedName qName((yyvsp[(3) - (4)].str));
        const XMLCh* prefix=qName.getPrefix();
        if(prefix == NULL || *prefix == 0)
          yyerror((yylsp[(3) - (4)]), "The pragma name must have a prefix [err:XPST0081]");

        try {
          LOCATION((yylsp[(3) - (4)]), loc);
          CONTEXT->getUriBoundToPrefix(prefix, &loc);
        }
        catch(NamespaceLookupException&) {
          yyerror((yylsp[(3) - (4)]), "The pragma name is using an undefined namespace prefix [err:XPST0081]");
        }
      }
    break;

  case 495:
#line 3751 "../src/parser/XQParser.y"
    {
    (yyval.str) = (yyvsp[(2) - (2)].str);
  }
    break;

  case 497:
#line 3760 "../src/parser/XQParser.y"
    {
    XQNav *nav = GET_NAVIGATION((yylsp[(1) - (2)]), (yyvsp[(1) - (2)].astNode));
    nav->addStep((yyvsp[(2) - (2)].astNode));
    (yyval.astNode) = nav;
  }
    break;

  case 498:
#line 3766 "../src/parser/XQParser.y"
    {
    XQNav *nav = WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQNav(MEMMGR));

    VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
    FunctionRoot *root = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FunctionRoot(args, MEMMGR));

    SequenceType *documentNode = WRAP((yylsp[(1) - (2)]), new (MEMMGR)
      SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT)));

    nav->addStep(WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQTreatAs(root, documentNode, MEMMGR)));

    NodeTest *step = new (MEMMGR) NodeTest();
    step->setTypeWildcard();
    step->setNameWildcard();
    step->setNamespaceWildcard();
    nav->addStep(WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQStep(XQStep::DESCENDANT_OR_SELF, step, MEMMGR)));

    nav->addStep((yyvsp[(2) - (2)].astNode));
    (yyval.astNode) = nav;
  }
    break;

  case 500:
#line 3791 "../src/parser/XQParser.y"
    {
    VectorOfASTNodes args(XQillaAllocator<ASTNode*>(MEMMGR));
    FunctionRoot *root = WRAP((yylsp[(1) - (1)]), new (MEMMGR) FunctionRoot(args, MEMMGR));

    SequenceType *documentNode = WRAP((yylsp[(1) - (1)]), new (MEMMGR)
      SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT)));

    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQTreatAs(root, documentNode, MEMMGR));
  }
    break;

  case 501:
#line 3805 "../src/parser/XQParser.y"
    {
    XQNav *nav = GET_NAVIGATION((yylsp[(1) - (3)]), (yyvsp[(1) - (3)].astNode));
    nav->addStep((yyvsp[(3) - (3)].astNode));
    (yyval.astNode) = nav;
  }
    break;

  case 502:
#line 3811 "../src/parser/XQParser.y"
    {
    XQNav *nav = GET_NAVIGATION((yylsp[(1) - (3)]), (yyvsp[(1) - (3)].astNode));

    NodeTest *step = new (MEMMGR) NodeTest();
    step->setTypeWildcard();
    step->setNameWildcard();
    step->setNamespaceWildcard();
    nav->addStep(WRAP((yylsp[(2) - (3)]), new (MEMMGR) XQStep(XQStep::DESCENDANT_OR_SELF, step, MEMMGR)));
    nav->addStep((yyvsp[(3) - (3)].astNode));

    (yyval.astNode) = nav;
  }
    break;

  case 506:
#line 3832 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = XQPredicate::addPredicates((yyvsp[(1) - (2)].astNode), (yyvsp[(2) - (2)].predicates));
  }
    break;

  case 507:
#line 3836 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = XQPredicate::addReversePredicates((yyvsp[(1) - (2)].astNode), (yyvsp[(2) - (2)].predicates));
    (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQDocumentOrder((yyval.astNode), MEMMGR));
  }
    break;

  case 508:
#line 3845 "../src/parser/XQParser.y"
    {
    if(!(yyvsp[(2) - (2)].nodeTest)->isNodeTypeSet()) {
      switch((yyvsp[(1) - (2)].axis)) {
      case XQStep::NAMESPACE: (yyvsp[(2) - (2)].nodeTest)->setNodeType(Node::namespace_string); break;
      case XQStep::ATTRIBUTE: (yyvsp[(2) - (2)].nodeTest)->setNodeType(Node::attribute_string); break;
      default: (yyvsp[(2) - (2)].nodeTest)->setNodeType(Node::element_string); break;
      }
    }

    (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQStep((yyvsp[(1) - (2)].axis),(yyvsp[(2) - (2)].nodeTest),MEMMGR));
  }
    break;

  case 510:
#line 3868 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::CHILD;
  }
    break;

  case 511:
#line 3872 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::DESCENDANT;
  }
    break;

  case 512:
#line 3876 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::ATTRIBUTE;
  }
    break;

  case 513:
#line 3880 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::SELF;
  }
    break;

  case 514:
#line 3884 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::DESCENDANT_OR_SELF;
  }
    break;

  case 515:
#line 3888 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::FOLLOWING_SIBLING;
  }
    break;

  case 516:
#line 3892 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::FOLLOWING;
  }
    break;

  case 517:
#line 3896 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::NAMESPACE;
  }
    break;

  case 518:
#line 3904 "../src/parser/XQParser.y"
    {
    if(!(yyvsp[(2) - (2)].nodeTest)->isNodeTypeSet()) {
      (yyvsp[(2) - (2)].nodeTest)->setNodeType(Node::attribute_string);
    }

    (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQStep(XQStep::ATTRIBUTE, (yyvsp[(2) - (2)].nodeTest), MEMMGR));
  }
    break;

  case 519:
#line 3912 "../src/parser/XQParser.y"
    {
    XQStep::Axis axis = XQStep::CHILD;
    SequenceType::ItemType *itemtype = (yyvsp[(1) - (1)].nodeTest)->getItemType();
    if(itemtype != 0 && itemtype->getItemTestType() == SequenceType::ItemType::TEST_ATTRIBUTE) {
      axis = XQStep::ATTRIBUTE;
    }
    else if(!(yyvsp[(1) - (1)].nodeTest)->isNodeTypeSet()) {
      (yyvsp[(1) - (1)].nodeTest)->setNodeType(Node::element_string);
    }

    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQStep(axis, (yyvsp[(1) - (1)].nodeTest), MEMMGR));
  }
    break;

  case 520:
#line 3929 "../src/parser/XQParser.y"
    {
    if(!(yyvsp[(2) - (2)].nodeTest)->isNodeTypeSet()) {
      (yyvsp[(2) - (2)].nodeTest)->setNodeType(Node::element_string);
    }

    (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQStep((yyvsp[(1) - (2)].axis), (yyvsp[(2) - (2)].nodeTest), MEMMGR));
  }
    break;

  case 522:
#line 3946 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::PARENT;
  }
    break;

  case 523:
#line 3950 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::ANCESTOR;
  }
    break;

  case 524:
#line 3954 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::PRECEDING_SIBLING;
  }
    break;

  case 525:
#line 3958 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::PRECEDING;
  }
    break;

  case 526:
#line 3962 "../src/parser/XQParser.y"
    {
    (yyval.axis) = XQStep::ANCESTOR_OR_SELF;
  }
    break;

  case 527:
#line 3970 "../src/parser/XQParser.y"
    {
    NodeTest *step = new (MEMMGR) NodeTest();
    step->setNameWildcard();
    step->setNamespaceWildcard();
    step->setTypeWildcard();
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQStep(XQStep::PARENT, step, MEMMGR));
  }
    break;

  case 528:
#line 3982 "../src/parser/XQParser.y"
    {
    (yyval.nodeTest) = new (MEMMGR) NodeTest();
    (yyval.nodeTest)->setItemType((yyvsp[(1) - (1)].itemType));
  }
    break;

  case 530:
#line 3992 "../src/parser/XQParser.y"
    {
    NodeTest *step = new (MEMMGR) NodeTest();
    step->setNodePrefix((yyvsp[(1) - (1)].qName)->getPrefix());
    step->setNodeName((yyvsp[(1) - (1)].qName)->getName());
    (yyval.nodeTest) = step;
  }
    break;

  case 532:
#line 4004 "../src/parser/XQParser.y"
    {
    NodeTest *step = new (MEMMGR) NodeTest();
    step->setNameWildcard();
    step->setNamespaceWildcard();
    (yyval.nodeTest) = step;
  }
    break;

  case 533:
#line 4011 "../src/parser/XQParser.y"
    {
    NodeTest *step = new (MEMMGR) NodeTest();
    step->setNodePrefix((yyvsp[(1) - (1)].str));
    step->setNameWildcard();
    (yyval.nodeTest) = step;
  }
    break;

  case 534:
#line 4018 "../src/parser/XQParser.y"
    {
    NodeTest *step = new (MEMMGR) NodeTest();
    step->setNodeName((yyvsp[(1) - (1)].str));
    step->setNamespaceWildcard();
    (yyval.nodeTest) = step;
  }
    break;

  case 535:
#line 4029 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = XQPredicate::addPredicates((yyvsp[(1) - (2)].astNode), (yyvsp[(2) - (2)].predicates));
  }
    break;

  case 536:
#line 4038 "../src/parser/XQParser.y"
    {
    (yyval.predicates) = new (MEMMGR) VectorOfPredicates(MEMMGR);
  }
    break;

  case 537:
#line 4042 "../src/parser/XQParser.y"
    {
    XQPredicate *pred = WRAP((yylsp[(2) - (4)]), new (MEMMGR) XQPredicate((yyvsp[(3) - (4)].astNode), MEMMGR));
    (yyvsp[(1) - (4)].predicates)->push_back(pred);
    (yyval.predicates) = (yyvsp[(1) - (4)].predicates); 
  }
    break;

  case 553:
#line 4073 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQVariable((yyvsp[(2) - (2)].str), MEMMGR));
  }
    break;

  case 555:
#line 4084 "../src/parser/XQParser.y"
    { 
    (yyval.astNode) = (yyvsp[(2) - (3)].astNode);
  }
    break;

  case 556:
#line 4088 "../src/parser/XQParser.y"
    { 
    (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQSequence(MEMMGR));
  }
    break;

  case 557:
#line 4096 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQContextItem(MEMMGR));
  }
    break;

  case 558:
#line 4104 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(OrderedExpr, (yylsp[(1) - (4)]));

    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQOrderingChange(StaticContext::ORDERING_ORDERED, (yyvsp[(3) - (4)].astNode), MEMMGR));
  }
    break;

  case 559:
#line 4114 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(UnorderedExpr, (yylsp[(1) - (4)]));

    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQOrderingChange(StaticContext::ORDERING_UNORDERED, (yyvsp[(3) - (4)].astNode), MEMMGR));
  }
    break;

  case 560:
#line 4124 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQFunctionCall((yyvsp[(1) - (3)].str), NULL, MEMMGR));
  }
    break;

  case 561:
#line 4128 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQFunctionCall((yyvsp[(1) - (4)].str), (yyvsp[(3) - (4)].itemList), MEMMGR));
  }
    break;

  case 562:
#line 4135 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].itemList)->push_back((yyvsp[(3) - (3)].astNode));
    (yyval.itemList) = (yyvsp[(1) - (3)].itemList);
  }
    break;

  case 563:
#line 4140 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    (yyval.itemList)->push_back((yyvsp[(1) - (1)].astNode));
  }
    break;

  case 564:
#line 4149 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(Constructor, (yylsp[(1) - (1)]));
  }
    break;

  case 565:
#line 4153 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(Constructor, (yylsp[(1) - (1)]));
  }
    break;

  case 569:
#line 4166 "../src/parser/XQParser.y"
    { 
    VectorOfASTNodes* content = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    ASTNode *name = WRAP((yylsp[(2) - (5)]), new (MEMMGR) XQDirectName((yyvsp[(2) - (5)].str), /*isAttr*/false, MEMMGR));
    (yyval.astNode) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) XQElementConstructor(name, (yyvsp[(3) - (5)].itemList), content, MEMMGR));
  }
    break;

  case 570:
#line 4172 "../src/parser/XQParser.y"
    { 
    if(!XPath2Utils::equals((yyvsp[(2) - (10)].str), (yyvsp[(8) - (10)].str)))
      yyerror((yylsp[(7) - (10)]), "Close tag does not match open tag");
    ASTNode *name = WRAP((yylsp[(2) - (10)]), new (MEMMGR) XQDirectName((yyvsp[(2) - (10)].str), /*isAttr*/false, MEMMGR));
    (yyval.astNode) = WRAP((yylsp[(1) - (10)]), new (MEMMGR) XQElementConstructor(name, (yyvsp[(3) - (10)].itemList), (yyvsp[(6) - (10)].itemList), MEMMGR));
  }
    break;

  case 573:
#line 4185 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
  }
    break;

  case 574:
#line 4189 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (7)].itemList);

    ASTNode *name = WRAP((yylsp[(3) - (7)]), new (MEMMGR) XQDirectName((yyvsp[(3) - (7)].str), /*isAttr*/true, MEMMGR));
    ASTNode *attrItem = WRAP((yylsp[(3) - (7)]), new (MEMMGR) XQAttributeConstructor(name, (yyvsp[(7) - (7)].itemList),MEMMGR));

    (yyval.itemList)->push_back(attrItem);
  }
    break;

  case 575:
#line 4198 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (7)].itemList);

    ASTNode *name = WRAP((yylsp[(3) - (7)]), new (MEMMGR) XQDirectName((yyvsp[(3) - (7)].str), /*isAttr*/true, MEMMGR));
    ASTNode *attrItem = WRAP((yylsp[(3) - (7)]), new (MEMMGR) XQAttributeConstructor(name, (yyvsp[(7) - (7)].itemList),MEMMGR));

    (yyval.itemList)->insert((yyval.itemList)->begin(), attrItem);
  }
    break;

  case 578:
#line 4214 "../src/parser/XQParser.y"
    { 
    (yyval.itemList) = (yyvsp[(2) - (3)].itemList);
  }
    break;

  case 579:
#line 4218 "../src/parser/XQParser.y"
    { 
    (yyval.itemList) = (yyvsp[(2) - (3)].itemList);
  }
    break;

  case 580:
#line 4225 "../src/parser/XQParser.y"
    { 
    (yyval.itemList) = (yyvsp[(2) - (3)].itemList);
  }
    break;

  case 581:
#line 4229 "../src/parser/XQParser.y"
    { 
    (yyval.itemList) = (yyvsp[(2) - (3)].itemList);
  }
    break;

  case 582:
#line 4239 "../src/parser/XQParser.y"
    { 
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
  }
    break;

  case 583:
#line 4243 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back((yyvsp[(2) - (2)].astNode));
  }
    break;

  case 584:
#line 4248 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQLiteral(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                               SchemaSymbols::fgDT_STRING,
                               (yyvsp[(2) - (2)].str), AnyAtomicType::STRING, MEMMGR)));
  }
    break;

  case 585:
#line 4258 "../src/parser/XQParser.y"
    { 
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
  }
    break;

  case 586:
#line 4262 "../src/parser/XQParser.y"
    {
    yyerror((yylsp[(2) - (2)]), "Namespace URI of a namespace declaration must be a literal [err:XQST0022]");
  }
    break;

  case 587:
#line 4266 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQLiteral(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                               SchemaSymbols::fgDT_STRING,
                               (yyvsp[(2) - (2)].str), AnyAtomicType::STRING, MEMMGR)));
  }
    break;

  case 588:
#line 4279 "../src/parser/XQParser.y"
    { 
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
  }
    break;

  case 589:
#line 4283 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back((yyvsp[(2) - (2)].astNode));
  }
    break;

  case 590:
#line 4288 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQLiteral(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                               SchemaSymbols::fgDT_STRING,
                               (yyvsp[(2) - (2)].str), AnyAtomicType::STRING, MEMMGR)));
  }
    break;

  case 591:
#line 4298 "../src/parser/XQParser.y"
    { 
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
  }
    break;

  case 592:
#line 4302 "../src/parser/XQParser.y"
    {
    yyerror((yylsp[(2) - (2)]), "Namespace URI of a namespace declaration must be a literal [err:XQST0022]");
  }
    break;

  case 593:
#line 4306 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQLiteral(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                               SchemaSymbols::fgDT_STRING,
                               (yyvsp[(2) - (2)].str), AnyAtomicType::STRING, MEMMGR)));
  }
    break;

  case 594:
#line 4323 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR)); 
  }
    break;

  case 595:
#line 4327 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back((yyvsp[(2) - (2)].astNode));
  }
    break;

  case 596:
#line 4332 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back((yyvsp[(2) - (2)].astNode));
  }
    break;

  case 597:
#line 4337 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    (yyval.itemList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQLiteral(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                               SchemaSymbols::fgDT_STRING,
                               (yyvsp[(2) - (2)].str), AnyAtomicType::STRING, MEMMGR)));
  }
    break;

  case 598:
#line 4344 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = (yyvsp[(1) - (2)].itemList);
    if(CONTEXT->getPreserveBoundarySpace()) {
      (yyval.itemList)->push_back(WRAP((yylsp[(2) - (2)]), new (MEMMGR) XQLiteral(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                                 SchemaSymbols::fgDT_STRING,
                                 (yyvsp[(2) - (2)].str), AnyAtomicType::STRING, MEMMGR)));
    }
  }
    break;

  case 599:
#line 4357 "../src/parser/XQParser.y"
    {
    ASTNode *value = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQLiteral(
                  SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                  SchemaSymbols::fgDT_STRING,
                  (yyvsp[(2) - (3)].str), AnyAtomicType::STRING,
                  MEMMGR));
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQCommentConstructor(value, MEMMGR));
  }
    break;

  case 600:
#line 4370 "../src/parser/XQParser.y"
    {
    (yyval.str) = (XMLCh*)XMLUni::fgZeroLenString;
  }
    break;

  case 602:
#line 4379 "../src/parser/XQParser.y"
    {
    ASTNode *value = WRAP((yylsp[(3) - (3)]), new (MEMMGR) XQLiteral(
                  SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                  SchemaSymbols::fgDT_STRING,
                  (yyvsp[(3) - (3)].str), AnyAtomicType::STRING,
                  MEMMGR));
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQPIConstructor(
                    WRAP((yylsp[(2) - (3)]), new (MEMMGR) XQLiteral(
                  SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                  SchemaSymbols::fgDT_STRING,
                  (yyvsp[(2) - (3)].str), AnyAtomicType::STRING,
                  MEMMGR)), 
                  value, MEMMGR));
  }
    break;

  case 604:
#line 4399 "../src/parser/XQParser.y"
    {
    (yyval.str) = (yyvsp[(2) - (2)].str);
  }
    break;

  case 611:
#line 4422 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQDocumentConstructor((yyvsp[(3) - (4)].astNode), MEMMGR));
  }
    break;

  case 612:
#line 4430 "../src/parser/XQParser.y"
    {
    VectorOfASTNodes* empty = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQElementConstructor((yyvsp[(2) - (3)].astNode), empty, (yyvsp[(3) - (3)].itemList), MEMMGR));
  }
    break;

  case 613:
#line 4438 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQDirectName((yyvsp[(1) - (1)].str), /*isAttr*/false, MEMMGR));
  }
    break;

  case 614:
#line 4442 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(2) - (3)].astNode);
  }
    break;

  case 615:
#line 4450 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
  }
    break;

  case 616:
#line 4454 "../src/parser/XQParser.y"
    {
    (yyval.itemList) = new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR));
    (yyval.itemList)->push_back((yyvsp[(2) - (3)].astNode));
  }
    break;

  case 617:
#line 4463 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQAttributeConstructor((yyvsp[(2) - (3)].astNode), (yyvsp[(3) - (3)].itemList), MEMMGR));
  }
    break;

  case 618:
#line 4471 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQTextConstructor((yyvsp[(3) - (4)].astNode), MEMMGR));
  }
    break;

  case 619:
#line 4479 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQCommentConstructor((yyvsp[(3) - (4)].astNode), MEMMGR));
  }
    break;

  case 620:
#line 4487 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQPIConstructor((yyvsp[(2) - (3)].astNode), (yyvsp[(3) - (3)].astNode), MEMMGR));
  }
    break;

  case 621:
#line 4494 "../src/parser/XQParser.y"
    {
    // Check for a colon
    for(XMLCh *tmp = (yyvsp[(1) - (1)].str); *tmp; ++tmp)
      if(*tmp == ':') yyerror((yylsp[(1) - (1)]), "Expecting an NCName, found a QName");

    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQLiteral(SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                               SchemaSymbols::fgDT_STRING,
                               (yyvsp[(1) - (1)].str), AnyAtomicType::STRING, MEMMGR));
  }
    break;

  case 622:
#line 4504 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(2) - (3)].astNode);
  }
    break;

  case 623:
#line 4511 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) XQSequence(MEMMGR));
  }
    break;

  case 624:
#line 4515 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(2) - (3)].astNode);
  }
    break;

  case 625:
#line 4523 "../src/parser/XQParser.y"
    {
    SequenceType* seq = WRAP((yylsp[(1) - (2)]), new (MEMMGR) SequenceType());
    seq->setItemType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE, NULL, (yyvsp[(1) - (2)].qName)));
    seq->setOccurrence((yyvsp[(2) - (2)].occurrence));
    (yyval.sequenceType) = seq;
  }
    break;

  case 626:
#line 4533 "../src/parser/XQParser.y"
    {
    (yyval.occurrence) = SequenceType::EXACTLY_ONE;
  }
    break;

  case 627:
#line 4537 "../src/parser/XQParser.y"
    {
    (yyval.occurrence) = SequenceType::QUESTION_MARK;
  }
    break;

  case 628:
#line 4545 "../src/parser/XQParser.y"
    {
    (yyval.sequenceType) = WRAP((yyloc), new (MEMMGR) SequenceType(new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING), SequenceType::STAR));
  }
    break;

  case 629:
#line 4549 "../src/parser/XQParser.y"
    {
    REJECT_NOT_XQUERY(TypeDeclaration, (yylsp[(1) - (2)]));

    (yyval.sequenceType) = (yyvsp[(2) - (2)].sequenceType);
  }
    break;

  case 630:
#line 4559 "../src/parser/XQParser.y"
    {
    SequenceType* seq = WRAP((yylsp[(1) - (2)]), new (MEMMGR) SequenceType());
    seq->setItemType((yyvsp[(1) - (2)].itemType));
    seq->setOccurrence((yyvsp[(2) - (2)].occurrence));
    (yyval.sequenceType) = seq;
  }
    break;

  case 631:
#line 4566 "../src/parser/XQParser.y"
    { 
    (yyval.sequenceType) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) SequenceType()); 
  }
    break;

  case 632:
#line 4575 "../src/parser/XQParser.y"
    { (yyval.occurrence) = SequenceType::EXACTLY_ONE; }
    break;

  case 633:
#line 4577 "../src/parser/XQParser.y"
    { (yyval.occurrence) = SequenceType::STAR; }
    break;

  case 634:
#line 4579 "../src/parser/XQParser.y"
    { (yyval.occurrence) = SequenceType::PLUS; }
    break;

  case 635:
#line 4581 "../src/parser/XQParser.y"
    { (yyval.occurrence) = SequenceType::QUESTION_MARK; }
    break;

  case 636:
#line 4587 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATOMIC_TYPE, NULL, (yyvsp[(1) - (1)].qName));
  }
    break;

  case 637:
#line 4591 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ANYTHING);
  }
    break;

  case 651:
#line 4626 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_NODE);
  }
    break;

  case 652:
#line 4634 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_DOCUMENT);
  }
    break;

  case 653:
#line 4638 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = (yyvsp[(3) - (4)].itemType);
    (yyval.itemType)->setItemTestType(SequenceType::ItemType::TEST_DOCUMENT);
  }
    break;

  case 654:
#line 4643 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = (yyvsp[(3) - (4)].itemType);
    (yyval.itemType)->setItemTestType(SequenceType::ItemType::TEST_SCHEMA_DOCUMENT);
  }
    break;

  case 655:
#line 4652 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_TEXT);
  }
    break;

  case 656:
#line 4660 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_COMMENT);
  }
    break;

  case 657:
#line 4668 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_PI);
  }
    break;

  case 658:
#line 4672 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_PI, new (MEMMGR) QualifiedName((yyvsp[(3) - (4)].str), MEMMGR));
  }
    break;

  case 659:
#line 4676 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_PI, new (MEMMGR) QualifiedName((yyvsp[(3) - (4)].str), MEMMGR));
  }
    break;

  case 660:
#line 4684 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATTRIBUTE);
  }
    break;

  case 661:
#line 4688 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATTRIBUTE,(yyvsp[(3) - (4)].qName));
  }
    break;

  case 662:
#line 4692 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ATTRIBUTE,(yyvsp[(3) - (6)].qName),(yyvsp[(5) - (6)].qName));
  }
    break;

  case 664:
#line 4701 "../src/parser/XQParser.y"
    {
    (yyval.qName) = NULL;
  }
    break;

  case 665:
#line 4709 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_SCHEMA_ATTRIBUTE,(yyvsp[(3) - (4)].qName));
  }
    break;

  case 667:
#line 4722 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT);
  }
    break;

  case 668:
#line 4726 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT,(yyvsp[(3) - (4)].qName));
  }
    break;

  case 669:
#line 4730 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT,(yyvsp[(3) - (6)].qName),(yyvsp[(5) - (6)].qName));
  }
    break;

  case 670:
#line 4734 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_ELEMENT,(yyvsp[(3) - (7)].qName),(yyvsp[(5) - (7)].qName));
    (yyval.itemType)->setAllowNilled(true);
  }
    break;

  case 672:
#line 4744 "../src/parser/XQParser.y"
    {
    (yyval.qName) = NULL;
  }
    break;

  case 673:
#line 4752 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_SCHEMA_ELEMENT,(yyvsp[(3) - (4)].qName));
  }
    break;

  case 678:
#line 4772 "../src/parser/XQParser.y"
    {
    // the string must be whitespace-normalized
    XMLString::collapseWS((yyvsp[(1) - (1)].str), MEMMGR);
    if((yyvsp[(1) - (1)].str) && *(yyvsp[(1) - (1)].str) && !XPath2Utils::isValidURI((yyvsp[(1) - (1)].str), MEMMGR))
      yyerror((yylsp[(1) - (1)]), "The URI literal is not valid [err:XQST0046]");
    (yyval.str) = (yyvsp[(1) - (1)].str);
  }
    break;

  case 679:
#line 4784 "../src/parser/XQParser.y"
    {
    // TBD FTSelectionOptions and weight
    (yyval.ftselection) = (yyvsp[(1) - (4)].ftselection);

    for(VectorOfFTOptions::iterator i = (yyvsp[(2) - (4)].ftoptionlist)->begin();
        i != (yyvsp[(2) - (4)].ftoptionlist)->end(); ++i) {
      (*i)->setArgument((yyval.ftselection));
      (yyval.ftselection) = *i;
    }
/*     delete $2; */
  }
    break;

  case 680:
#line 4796 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = (yyvsp[(1) - (2)].ftselection);

    VectorOfFTOptions *options = (yyvsp[(2) - (2)].ftoptionlist);
    for(VectorOfFTOptions::iterator i = options->begin();
        i != options->end(); ++i) {
      (*i)->setArgument((yyval.ftselection));
      (yyval.ftselection) = *i;
    }
/*     delete options; */
  }
    break;

  case 681:
#line 4811 "../src/parser/XQParser.y"
    {
    (yyval.ftoptionlist) = new (MEMMGR) VectorOfFTOptions(XQillaAllocator<FTOption*>(MEMMGR));
  }
    break;

  case 682:
#line 4815 "../src/parser/XQParser.y"
    {
    (yyval.ftoptionlist) = (yyvsp[(1) - (2)].ftoptionlist);
  }
    break;

  case 683:
#line 4819 "../src/parser/XQParser.y"
    {
    if((yyvsp[(2) - (2)].ftoption) != NULL)
      (yyvsp[(1) - (2)].ftoptionlist)->push_back((yyvsp[(2) - (2)].ftoption));
    (yyval.ftoptionlist) = (yyvsp[(1) - (2)].ftoptionlist);
  }
    break;

  case 684:
#line 4829 "../src/parser/XQParser.y"
    {
    if((yyvsp[(1) - (3)].ftselection)->getType() == FTSelection::OR) {
      FTOr *op = (FTOr*)(yyvsp[(1) - (3)].ftselection);
      op->addArg((yyvsp[(3) - (3)].ftselection));
      (yyval.ftselection) = op;
    }
    else {
      (yyval.ftselection) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) FTOr((yyvsp[(1) - (3)].ftselection), (yyvsp[(3) - (3)].ftselection), MEMMGR));
    }
  }
    break;

  case 686:
#line 4845 "../src/parser/XQParser.y"
    {
    if((yyvsp[(1) - (3)].ftselection)->getType() == FTSelection::AND) {
      FTAnd *op = (FTAnd*)(yyvsp[(1) - (3)].ftselection);
      op->addArg((yyvsp[(3) - (3)].ftselection));
      (yyval.ftselection) = op;
    }
    else {
      (yyval.ftselection) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) FTAnd((yyvsp[(1) - (3)].ftselection), (yyvsp[(3) - (3)].ftselection), MEMMGR));
    }
  }
    break;

  case 688:
#line 4861 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[(2) - (4)]), new (MEMMGR) FTMildnot((yyvsp[(1) - (4)].ftselection), (yyvsp[(4) - (4)].ftselection), MEMMGR));
  }
    break;

  case 690:
#line 4870 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTUnaryNot((yyvsp[(2) - (2)].ftselection), MEMMGR));
  }
    break;

  case 692:
#line 4879 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = (yyvsp[(2) - (3)].ftselection);
  }
    break;

  case 694:
#line 4888 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTWords((yyvsp[(1) - (2)].astNode), (yyvsp[(2) - (2)].ftanyalloption), MEMMGR));
  }
    break;

  case 695:
#line 4892 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTWords((yyvsp[(1) - (2)].astNode), (yyvsp[(2) - (2)].ftanyalloption), MEMMGR));
  }
    break;

  case 696:
#line 4896 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTWords((yyvsp[(1) - (2)].astNode), (yyvsp[(2) - (2)].ftanyalloption), MEMMGR));
  }
    break;

  case 697:
#line 4900 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTWords((yyvsp[(1) - (2)].astNode), (yyvsp[(2) - (2)].ftanyalloption), MEMMGR));
  }
    break;

  case 698:
#line 4904 "../src/parser/XQParser.y"
    {
    (yyval.ftselection) = WRAP((yylsp[(2) - (4)]), new (MEMMGR) FTWords((yyvsp[(2) - (4)].astNode), (yyvsp[(4) - (4)].ftanyalloption), MEMMGR));
  }
    break;

  case 699:
#line 4918 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) FTOrder(MEMMGR));
  }
    break;

  case 700:
#line 4922 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) FTWindow((yyvsp[(2) - (3)].astNode), (yyvsp[(3) - (3)].ftunit), MEMMGR));
  }
    break;

  case 701:
#line 4926 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) FTDistance((yyvsp[(2) - (3)].ftrange), (yyvsp[(3) - (3)].ftunit), MEMMGR));
  }
    break;

  case 702:
#line 4930 "../src/parser/XQParser.y"
    {
    std::cerr << "occurs" << std::endl;
    (yyval.ftoption) = NULL;
  }
    break;

  case 703:
#line 4935 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTScope(FTScope::SAME, (yyvsp[(2) - (2)].ftunit), MEMMGR));
  }
    break;

  case 704:
#line 4939 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTScope(FTScope::DIFFERENT, (yyvsp[(2) - (2)].ftunit), MEMMGR));
  }
    break;

  case 705:
#line 4943 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTContent(FTContent::AT_START, MEMMGR));
  }
    break;

  case 706:
#line 4947 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTContent(FTContent::AT_END, MEMMGR));
  }
    break;

  case 707:
#line 4951 "../src/parser/XQParser.y"
    {
    (yyval.ftoption) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) FTContent(FTContent::ENTIRE_CONTENT, MEMMGR));
  }
    break;

  case 715:
#line 4979 "../src/parser/XQParser.y"
    {
    std::cerr << "lowercase" << std::endl;
  }
    break;

  case 716:
#line 4983 "../src/parser/XQParser.y"
    {
    std::cerr << "uppercase" << std::endl;
  }
    break;

  case 717:
#line 4987 "../src/parser/XQParser.y"
    {
    std::cerr << "case sensitive" << std::endl;
  }
    break;

  case 718:
#line 4991 "../src/parser/XQParser.y"
    {
    std::cerr << "case insensitive" << std::endl;
  }
    break;

  case 719:
#line 5002 "../src/parser/XQParser.y"
    {
    std::cerr << "with diacritics" << std::endl;
  }
    break;

  case 720:
#line 5006 "../src/parser/XQParser.y"
    {
    std::cerr << "without diacritics" << std::endl;
  }
    break;

  case 721:
#line 5010 "../src/parser/XQParser.y"
    {
    std::cerr << "diacritics sensitive" << std::endl;
  }
    break;

  case 722:
#line 5014 "../src/parser/XQParser.y"
    {
    std::cerr << "diacritics insensitive" << std::endl;
  }
    break;

  case 723:
#line 5022 "../src/parser/XQParser.y"
    {
    std::cerr << "with stemming" << std::endl;
  }
    break;

  case 724:
#line 5026 "../src/parser/XQParser.y"
    {
    std::cerr << "without stemming" << std::endl;
  }
    break;

  case 725:
#line 5036 "../src/parser/XQParser.y"
    {
    std::cerr << "with thesaurus" << std::endl;
  }
    break;

  case 726:
#line 5040 "../src/parser/XQParser.y"
    {
    std::cerr << "with thesaurus default" << std::endl;
  }
    break;

  case 727:
#line 5044 "../src/parser/XQParser.y"
    {
    std::cerr << "with thesaurus ()" << std::endl;
  }
    break;

  case 728:
#line 5048 "../src/parser/XQParser.y"
    {
    std::cerr << "with thesaurus (default)" << std::endl;
  }
    break;

  case 729:
#line 5052 "../src/parser/XQParser.y"
    {
    std::cerr << "without thesaurus" << std::endl;
  }
    break;

  case 730:
#line 5059 "../src/parser/XQParser.y"
    {
  }
    break;

  case 731:
#line 5062 "../src/parser/XQParser.y"
    {
  }
    break;

  case 732:
#line 5069 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral" << std::endl;
  }
    break;

  case 733:
#line 5073 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral relationship StringLiteral" << std::endl;
  }
    break;

  case 734:
#line 5077 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral levels" << std::endl;
  }
    break;

  case 735:
#line 5081 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral relationship StringLiteral levels" << std::endl;
  }
    break;

  case 736:
#line 5091 "../src/parser/XQParser.y"
    {
    std::cerr << "with stop words" << std::endl;
  }
    break;

  case 737:
#line 5095 "../src/parser/XQParser.y"
    {
    std::cerr << "without stop words" << std::endl;
  }
    break;

  case 738:
#line 5099 "../src/parser/XQParser.y"
    {
    std::cerr << "with default stop words" << std::endl;
  }
    break;

  case 739:
#line 5106 "../src/parser/XQParser.y"
    {
  }
    break;

  case 740:
#line 5109 "../src/parser/XQParser.y"
    {
  }
    break;

  case 741:
#line 5117 "../src/parser/XQParser.y"
    {
    std::cerr << "at StringLiteral" << std::endl;
  }
    break;

  case 742:
#line 5121 "../src/parser/XQParser.y"
    {
    std::cerr << "()" << std::endl;
  }
    break;

  case 743:
#line 5128 "../src/parser/XQParser.y"
    {
    std::cerr << "StringLiteral" << std::endl;
  }
    break;

  case 744:
#line 5132 "../src/parser/XQParser.y"
    {
    std::cerr << ", StringLiteral" << std::endl;
  }
    break;

  case 745:
#line 5140 "../src/parser/XQParser.y"
    {
    std::cerr << "union" << std::endl;
  }
    break;

  case 746:
#line 5144 "../src/parser/XQParser.y"
    {
    std::cerr << "except" << std::endl;
  }
    break;

  case 747:
#line 5152 "../src/parser/XQParser.y"
    {
    std::cerr << "language StringLiteral" << std::endl;
  }
    break;

  case 748:
#line 5160 "../src/parser/XQParser.y"
    {
    std::cerr << "with wildcards" << std::endl;
  }
    break;

  case 749:
#line 5164 "../src/parser/XQParser.y"
    {
    std::cerr << "without wildcards" << std::endl;
  }
    break;

  case 750:
#line 5172 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ANY;
  }
    break;

  case 751:
#line 5176 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ANY;
  }
    break;

  case 752:
#line 5180 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ANY_WORD;
  }
    break;

  case 753:
#line 5184 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ALL;
  }
    break;

  case 754:
#line 5188 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::ALL_WORDS;
  }
    break;

  case 755:
#line 5192 "../src/parser/XQParser.y"
    {
    (yyval.ftanyalloption) = FTWords::PHRASE;
  }
    break;

  case 756:
#line 5203 "../src/parser/XQParser.y"
    {
    (yyval.ftrange).type = FTRange::EXACTLY;
    (yyval.ftrange).arg1 = (yyvsp[(2) - (2)].astNode);
    (yyval.ftrange).arg2 = 0;
  }
    break;

  case 757:
#line 5209 "../src/parser/XQParser.y"
    {
    (yyval.ftrange).type = FTRange::AT_LEAST;
    (yyval.ftrange).arg1 = (yyvsp[(3) - (3)].astNode);
    (yyval.ftrange).arg2 = 0;
  }
    break;

  case 758:
#line 5215 "../src/parser/XQParser.y"
    {
    (yyval.ftrange).type = FTRange::AT_MOST;
    (yyval.ftrange).arg1 = (yyvsp[(3) - (3)].astNode);
    (yyval.ftrange).arg2 = 0;
  }
    break;

  case 759:
#line 5221 "../src/parser/XQParser.y"
    {
    (yyval.ftrange).type = FTRange::FROM_TO;
    (yyval.ftrange).arg1 = (yyvsp[(2) - (4)].astNode);
    (yyval.ftrange).arg2 = (yyvsp[(4) - (4)].astNode);
  }
    break;

  case 760:
#line 5231 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::WORDS;
  }
    break;

  case 761:
#line 5235 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::SENTENCES;
  }
    break;

  case 762:
#line 5239 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::PARAGRAPHS;
  }
    break;

  case 763:
#line 5247 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::SENTENCES;
  }
    break;

  case 764:
#line 5251 "../src/parser/XQParser.y"
    {
    (yyval.ftunit) = FTOption::PARAGRAPHS;
  }
    break;

  case 765:
#line 5259 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = (yyvsp[(3) - (3)].astNode);
  }
    break;

  case 766:
#line 5267 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_REVALIDATION_SPECIFIED, "revalidation", "XUST0003");
    CONTEXT->setRevalidationMode(DocumentCache::VALIDATION_STRICT);
  }
    break;

  case 767:
#line 5272 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_REVALIDATION_SPECIFIED, "revalidation", "XUST0003");
    CONTEXT->setRevalidationMode(DocumentCache::VALIDATION_LAX);
  }
    break;

  case 768:
#line 5277 "../src/parser/XQParser.y"
    {
    CHECK_SPECIFIED((yylsp[(1) - (3)]), BIT_REVALIDATION_SPECIFIED, "revalidation", "XUST0003");
    CONTEXT->setRevalidationMode(DocumentCache::VALIDATION_SKIP);
  }
    break;

  case 769:
#line 5289 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (6)]), new (MEMMGR) UInsertAsFirst((yyvsp[(2) - (6)].astNode), (yyvsp[(6) - (6)].astNode), MEMMGR));
  }
    break;

  case 770:
#line 5293 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (6)]), new (MEMMGR) UInsertAsLast((yyvsp[(2) - (6)].astNode), (yyvsp[(6) - (6)].astNode), MEMMGR));
  }
    break;

  case 771:
#line 5297 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) UInsertInto((yyvsp[(2) - (4)].astNode), (yyvsp[(4) - (4)].astNode), MEMMGR));
  }
    break;

  case 772:
#line 5301 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) UInsertAfter((yyvsp[(2) - (4)].astNode), (yyvsp[(4) - (4)].astNode), MEMMGR));
  }
    break;

  case 773:
#line 5305 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) UInsertBefore((yyvsp[(2) - (4)].astNode), (yyvsp[(4) - (4)].astNode), MEMMGR));
  }
    break;

  case 776:
#line 5316 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (2)]), new (MEMMGR) UDelete((yyvsp[(2) - (2)].astNode), MEMMGR));
  }
    break;

  case 779:
#line 5327 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (7)]), new (MEMMGR) UReplaceValueOf((yyvsp[(5) - (7)].astNode), (yyvsp[(7) - (7)].astNode), MEMMGR));
  }
    break;

  case 780:
#line 5331 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) UReplace((yyvsp[(3) - (5)].astNode), (yyvsp[(5) - (5)].astNode), MEMMGR));
  }
    break;

  case 781:
#line 5341 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) URename((yyvsp[(3) - (5)].astNode), (yyvsp[(5) - (5)].astNode), MEMMGR));
  }
    break;

  case 782:
#line 5349 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (6)]), new (MEMMGR) UTransform((yyvsp[(2) - (6)].copyBindingList), (yyvsp[(4) - (6)].astNode), (yyvsp[(6) - (6)].astNode), MEMMGR));
  }
    break;

  case 783:
#line 5356 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].copyBindingList)->push_back((yyvsp[(3) - (3)].copyBinding));
    (yyval.copyBindingList) = (yyvsp[(1) - (3)].copyBindingList);
  }
    break;

  case 784:
#line 5361 "../src/parser/XQParser.y"
    {
    (yyval.copyBindingList) = new (MEMMGR) VectorOfCopyBinding(XQillaAllocator<CopyBinding*>(MEMMGR));
    (yyval.copyBindingList)->push_back((yyvsp[(1) - (1)].copyBinding));
  }
    break;

  case 785:
#line 5369 "../src/parser/XQParser.y"
    {
    (yyval.copyBinding) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) CopyBinding(MEMMGR, (yyvsp[(2) - (4)].str), (yyvsp[(4) - (4)].astNode)));
  }
    break;

  case 786:
#line 5378 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQLiteral(
                  SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                  SchemaSymbols::fgDT_INTEGER,
                  (yyvsp[(1) - (1)].str), AnyAtomicType::DECIMAL,
                  MEMMGR));
  }
    break;

  case 787:
#line 5390 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQLiteral(
                  SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                  SchemaSymbols::fgDT_DECIMAL,
                  (yyvsp[(1) - (1)].str), AnyAtomicType::DECIMAL,
                  MEMMGR));
  }
    break;

  case 788:
#line 5402 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQLiteral(
                  SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                  SchemaSymbols::fgDT_DOUBLE,
                  (yyvsp[(1) - (1)].str), AnyAtomicType::DOUBLE,
                  MEMMGR));
  }
    break;

  case 789:
#line 5415 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQLiteral(
                  SchemaSymbols::fgURI_SCHEMAFORSCHEMA,
                  SchemaSymbols::fgDT_STRING,
                  (yyvsp[(1) - (1)].str), AnyAtomicType::STRING,
                  MEMMGR));
  }
    break;

  case 790:
#line 5426 "../src/parser/XQParser.y"
    {
    // Check for a colon
    for(XMLCh *tmp = (yyvsp[(1) - (1)].str); *tmp; ++tmp)
      if(*tmp == ':') yyerror((yylsp[(1) - (1)]), "Expecting an NCName, found a QName");
  }
    break;

  case 791:
#line 5435 "../src/parser/XQParser.y"
    {
    (yyval.qName) = new (MEMMGR) QualifiedName((yyvsp[(1) - (1)].str),MEMMGR);
  }
    break;

  case 792:
#line 5447 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) XQUserFunction((yyvsp[(4) - (5)].str), 0, 0, (yyvsp[(5) - (5)].astNode), 0, MEMMGR));
  }
    break;

  case 793:
#line 5451 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (7)]), new (MEMMGR) XQUserFunction((yyvsp[(4) - (7)].str), 0, (yyvsp[(5) - (7)].argSpecs), (yyvsp[(7) - (7)].astNode), (yyvsp[(6) - (7)].sequenceType), MEMMGR));
  }
    break;

  case 794:
#line 5455 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (8)]), new (MEMMGR) XQUserFunction((yyvsp[(4) - (8)].str), (yyvsp[(7) - (8)].itemList), 0, (yyvsp[(8) - (8)].astNode), 0, MEMMGR));
    (yyval.functDecl)->setModeList((yyvsp[(5) - (8)].modeList));
  }
    break;

  case 795:
#line 5460 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (10)]), new (MEMMGR) XQUserFunction((yyvsp[(4) - (10)].str), (yyvsp[(7) - (10)].itemList), (yyvsp[(8) - (10)].argSpecs), (yyvsp[(10) - (10)].astNode), (yyvsp[(9) - (10)].sequenceType), MEMMGR));
    (yyval.functDecl)->setModeList((yyvsp[(5) - (10)].modeList));
  }
    break;

  case 796:
#line 5465 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (6)]), new (MEMMGR) XQUserFunction(0, (yyvsp[(5) - (6)].itemList), 0, (yyvsp[(6) - (6)].astNode), 0, MEMMGR));
    (yyval.functDecl)->setModeList((yyvsp[(3) - (6)].modeList));
  }
    break;

  case 797:
#line 5470 "../src/parser/XQParser.y"
    {
    (yyval.functDecl) = WRAP((yylsp[(1) - (8)]), new (MEMMGR) XQUserFunction(0, (yyvsp[(5) - (8)].itemList), (yyvsp[(6) - (8)].argSpecs), (yyvsp[(8) - (8)].astNode), (yyvsp[(7) - (8)].sequenceType), MEMMGR));
    (yyval.functDecl)->setModeList((yyvsp[(3) - (8)].modeList));
  }
    break;

  case 798:
#line 5478 "../src/parser/XQParser.y"
    {
    (yyval.sequenceType) = 0;
  }
    break;

  case 799:
#line 5482 "../src/parser/XQParser.y"
    {
    (yyval.sequenceType) = (yyvsp[(2) - (2)].sequenceType);
  }
    break;

  case 800:
#line 5490 "../src/parser/XQParser.y"
    {
    (yyval.argSpecs) = 0;
  }
    break;

  case 801:
#line 5494 "../src/parser/XQParser.y"
    {
    (yyval.argSpecs) = (yyvsp[(2) - (3)].argSpecs);
  }
    break;

  case 802:
#line 5501 "../src/parser/XQParser.y"
    {
    (yyval.modeList) = new (MEMMGR) XQUserFunction::ModeList(XQillaAllocator<XQUserFunction::Mode*>(MEMMGR));
    (yyval.modeList)->push_back(WRAP((yyloc), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::DEFAULT)));
  }
    break;

  case 803:
#line 5506 "../src/parser/XQParser.y"
    {
    (yyval.modeList) = (yyvsp[(2) - (2)].modeList);
  }
    break;

  case 804:
#line 5514 "../src/parser/XQParser.y"
    {
    (yyval.modeList) = new (MEMMGR) XQUserFunction::ModeList(XQillaAllocator<XQUserFunction::Mode*>(MEMMGR));
    (yyval.modeList)->push_back((yyvsp[(1) - (1)].mode));
  }
    break;

  case 805:
#line 5519 "../src/parser/XQParser.y"
    {
    (yyvsp[(1) - (3)].modeList)->push_back((yyvsp[(3) - (3)].mode));
    (yyval.modeList) = (yyvsp[(1) - (3)].modeList);
  }
    break;

  case 806:
#line 5528 "../src/parser/XQParser.y"
    {
    (yyval.mode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode((yyvsp[(1) - (1)].str)));
  }
    break;

  case 807:
#line 5532 "../src/parser/XQParser.y"
    {
    (yyval.mode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::DEFAULT));
  }
    break;

  case 808:
#line 5536 "../src/parser/XQParser.y"
    {
    (yyval.mode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::ALL));
  }
    break;

  case 809:
#line 5544 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQCallTemplate((yyvsp[(3) - (3)].str), 0, MEMMGR));
  }
    break;

  case 810:
#line 5548 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (7)]), new (MEMMGR) XQCallTemplate((yyvsp[(3) - (7)].str), (yyvsp[(6) - (7)].templateArgs), MEMMGR));
  }
    break;

  case 811:
#line 5556 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQApplyTemplates((yyvsp[(3) - (3)].astNode), 0, 0, MEMMGR));
  }
    break;

  case 812:
#line 5560 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (7)]), new (MEMMGR) XQApplyTemplates((yyvsp[(3) - (7)].astNode), (yyvsp[(6) - (7)].templateArgs), 0, MEMMGR));
  }
    break;

  case 813:
#line 5564 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) XQApplyTemplates((yyvsp[(3) - (5)].astNode), 0, (yyvsp[(5) - (5)].mode), MEMMGR));
  }
    break;

  case 814:
#line 5568 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (9)]), new (MEMMGR) XQApplyTemplates((yyvsp[(3) - (9)].astNode), (yyvsp[(8) - (9)].templateArgs), (yyvsp[(5) - (9)].mode), MEMMGR));
  }
    break;

  case 815:
#line 5576 "../src/parser/XQParser.y"
    {
    (yyval.mode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode((yyvsp[(1) - (1)].str)));
  }
    break;

  case 816:
#line 5580 "../src/parser/XQParser.y"
    {
    (yyval.mode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::DEFAULT));
  }
    break;

  case 817:
#line 5584 "../src/parser/XQParser.y"
    {
    (yyval.mode) = WRAP((yylsp[(1) - (1)]), new (MEMMGR) XQUserFunction::Mode(XQUserFunction::Mode::CURRENT));
  }
    break;

  case 818:
#line 5592 "../src/parser/XQParser.y"
    {
    (yyval.templateArgs) = new (MEMMGR) TemplateArguments(XQillaAllocator<XQTemplateArgument*>(MEMMGR));
    (yyval.templateArgs)->push_back((yyvsp[(1) - (1)].templateArg));
  }
    break;

  case 819:
#line 5597 "../src/parser/XQParser.y"
    {
    (yyval.templateArgs) = (yyvsp[(1) - (3)].templateArgs);
    (yyval.templateArgs)->push_back((yyvsp[(3) - (3)].templateArg));
  }
    break;

  case 820:
#line 5607 "../src/parser/XQParser.y"
    {
    if((yyvsp[(3) - (5)].sequenceType) != 0)
      (yyvsp[(5) - (5)].astNode) = (yyvsp[(3) - (5)].sequenceType)->convertFunctionArg((yyvsp[(5) - (5)].astNode), CONTEXT, /*numericfunction*/false, (yyvsp[(3) - (5)].sequenceType));
    (yyval.templateArg) = WRAP((yylsp[(1) - (5)]), new (MEMMGR) XQTemplateArgument((yyvsp[(2) - (5)].str), (yyvsp[(5) - (5)].astNode), MEMMGR));
  }
    break;

  case 823:
#line 5624 "../src/parser/XQParser.y"
    {
    (yyval.astNode) = WRAP((yylsp[(1) - (3)]), new (MEMMGR) XQFunctionRef((yyvsp[(1) - (3)].str), atoi(UTF8((yyvsp[(3) - (3)].str))), MEMMGR));
  }
    break;

  case 824:
#line 5632 "../src/parser/XQParser.y"
    {
    XQUserFunction *func = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQUserFunction(0, (yyvsp[(2) - (4)].argSpecs), (yyvsp[(4) - (4)].astNode), (yyvsp[(3) - (4)].sequenceType), false, false, MEMMGR));
    (yyval.astNode) = WRAP((yylsp[(1) - (4)]), new (MEMMGR) XQInlineFunction(func, MEMMGR));
  }
    break;

  case 825:
#line 5641 "../src/parser/XQParser.y"
    {
    REJECT_NOT_EXTENSION(DereferencedFunctionCall, (yylsp[(1) - (3)]));
    (yyval.astNode) = WRAP((yylsp[(2) - (3)]), new (MEMMGR) XQFunctionDeref((yyvsp[(1) - (3)].astNode), new (MEMMGR) VectorOfASTNodes(XQillaAllocator<ASTNode*>(MEMMGR)), MEMMGR));
  }
    break;

  case 826:
#line 5646 "../src/parser/XQParser.y"
    {
    REJECT_NOT_EXTENSION(DereferencedFunctionCall, (yylsp[(1) - (4)]));
    (yyval.astNode) = WRAP((yylsp[(2) - (4)]), new (MEMMGR) XQFunctionDeref((yyvsp[(1) - (4)].astNode), (yyvsp[(3) - (4)].itemList), MEMMGR));
  }
    break;

  case 827:
#line 5655 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(SequenceType::ItemType::TEST_FUNCTION);
  }
    break;

  case 828:
#line 5659 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType(new (MEMMGR) VectorOfSequenceTypes(XQillaAllocator<SequenceType*>(MEMMGR)), (yyvsp[(5) - (5)].sequenceType));
  }
    break;

  case 829:
#line 5663 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = new (MEMMGR) SequenceType::ItemType((yyvsp[(3) - (6)].sequenceTypes), (yyvsp[(6) - (6)].sequenceType));
  }
    break;

  case 830:
#line 5670 "../src/parser/XQParser.y"
    {
    (yyval.sequenceTypes) = new (MEMMGR) VectorOfSequenceTypes(XQillaAllocator<SequenceType*>(MEMMGR));
    (yyval.sequenceTypes)->push_back((yyvsp[(1) - (1)].sequenceType));
  }
    break;

  case 831:
#line 5675 "../src/parser/XQParser.y"
    {
    (yyval.sequenceTypes) = (yyvsp[(1) - (3)].sequenceTypes);
    (yyval.sequenceTypes)->push_back((yyvsp[(3) - (3)].sequenceType));
  }
    break;

  case 832:
#line 5684 "../src/parser/XQParser.y"
    {
    (yyval.itemType) = (yyvsp[(2) - (3)].itemType);
  }
    break;


/* Line 1267 of yacc.c.  */
#line 10725 "../src/parser/XQParser.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 5718 "../src/parser/XQParser.y"


}  // namespace XQParser


