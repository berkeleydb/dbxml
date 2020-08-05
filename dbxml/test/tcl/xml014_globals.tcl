# See the file LICENSE for redistribution information.
#
# Copyright (c) 2000,2009 Oracle.  All rights reserved.
#
#
# Globals for test xml014
#
global content1
global expected1_1
global expected1_2
global expected1_3
global expected1_4
global expected1_5
global expected1_6
global expected1_7
global expected1_8
global expected1_9
global expected1_10
global expected1_11

# an empty root element
set content1 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root/>"

# new XmlModify xmod1 "/root" $XmlModify_Append $XmlModify_Element "new" "new content"
set expected1_1 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root><new>new content</new></root>"
# new XmlModify xmod1 "/root" $XmlModify_Append $XmlModify_Attribute "new" "foo"
set expected1_2 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root new=\"foo\"/>"
# new XmlModify xmod1 "/root" $XmlModify_Append $XmlModify_Attribute "new" ""
set expected1_2b "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root new=\"\"/>"
# new XmlModify xmod1 "/root" $XmlModify_Append $XmlModify_ProcessingInstruction "newPI" "PIcontent"
set expected1_3 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root><?newPI PIcontent?></root>"
# new XmlModify xmod1 "/root" $XmlModify_Append $XmlModify_Comment "" "comment content"
set expected1_4 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root><!--comment content--></root>"
# new XmlModify xmod1 "/root" $XmlModify_Append $XmlModify_Text "" "text content"
set expected1_5 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root>text content</root>"
# add element without name
set expected1_6 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root>val</root>"
# add multiple siblings in one shot (new in 2.3)
set expected1_7 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root><a/><a/><b>text</b></root>"
# multiple text/mixed content siblings (new in 2.3)
set expected1_8 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root>text1<!--comment--><a/>text2</root>"
# inserting extra root element
set expected1_9 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><new/><root/>"
# inserting extra root element (1)
set expected1_10 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><root/><new/>"
# remove root
set expected1_11 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"

global content2
global content2u
global expected2_1
global expected2_2
global expected2_3
global expected2_4
global expected2_5
global expected2_6
global expected2_7
global expected2_8
global expected2_9
global expected2_10
global expected2_11
#update (based on content2u)
global expected2_12
global expected2_13
global expected2_14
global expected2_15
global expected2_16

# a little structure.  2 "b" elements, some attrs, allows insertion before/after, append, remove
set content2 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a att1=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--></root>"

#removal cases:
# new XmlModify xmod1 "/root/b/@att1" $XmlModify_Remove $XmlModify_None "" ""
set expected2_1 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a att1=\"att1\">a content</a><b att2=\"att2\">b content 1</b><b att2=\"att2\">b content 2</b><!--comment--></root>"
# new XmlModify xmod1 "/root/b\[text()='b content 2'\]" $XmlModify_Remove $XmlModify_None "" ""
set expected2_2 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a att1=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><!--comment--></root>"
# new XmlModify xmod1 "/root/comment()" $XmlModify_Remove $XmlModify_None "" ""
set expected2_3 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a att1=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b></root>"
# new XmlModify xmod1 "/root/a/text()" $XmlModify_Remove $XmlModify_None "" ""
set expected2_4 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a att1=\"att1\"/><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--></root>"
# new XmlModify xmod1 "/root" $XmlModify_Append $XmlModify_Element "new" ""
set expected2_5 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a att1=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--><new/></root>"
# new XmlModify xmod1 "/root" $XmlModify_Append $XmlModify_Element "new" "" 0
set expected2_6 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><new/><a att1=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--></root>"
# new XmlModify xmod1 "/root" $XmlModify_Append $XmlModify_Element "new" "" 2
set expected2_7 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a att1=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><new/><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--></root>"
# new XmlModify xmod1 "/root/a" $XmlModify_InsertBefore $XmlModify_Element "new" ""
set expected2_8 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><new/><a att1=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--></root>"
# new XmlModify xmod1 "/root/a" $XmlModify_InsertAfter $XmlModify_Element "new" ""
set expected2_9 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a att1=\"att1\">a content</a><new/><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--></root>"
# new XmlModify xmod1 "/root/a" $XmlModify_Rename $XmlModify_None "x" ""
set expected2_10 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><x att1=\"att1\">a content</x><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--></root>"
#  new XmlModify xmod1 "/root/a/@att1" $XmlModify_Rename $XmlModify_None "att2" ""
set expected2_11 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a att2=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--></root>"

# test update -- need some mixed content, create content2u
set content2u "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a>a content 1<c/>a content 2</a><b>b content 1</b><!--comment--></root>"
#  new XmlModify xmod1 "/root/comment()" $XmlModify_Update $XmlModify_None "" "new comment"
set expected2_12 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a>a content 1<c/>a content 2</a><b>b content 1</b><!--new comment--></root>"
#  new XmlModify xmod1 "/root/a" $XmlModify_Update $XmlModify_None "" "new a text"
set expected2_13 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a>new a text</a><b>b content 1</b><!--comment--></root>"
#  new XmlModify xmod1 "/root/a" $XmlModify_Update $XmlModify_None "" ""
set expected2_14 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a/><b>b content 1</b><!--comment--></root>"
#  new XmlModify xmod1 "/root" $XmlModify_Update $XmlModify_None "" "new root text"
set expected2_15 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root>new root text</root>"
#  new XmlModify xmod1 "/root/b" $XmlModify_Update $XmlModify_None "" "new b text"
set expected2_16 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a>a content 1<c/>a content 2</a><b>new b text</b><!--comment--></root>"

# expected content for encoding changes
set expected2_1_ascii "<?xml version=\"1.0\" encoding=\"Ascii\" standalone=\"no\"?><root><a att1=\"att1\">a content</a><b att2=\"att2\">b content 1</b><b att2=\"att2\">b content 2</b><!--comment--></root>"
set expected2_5_windows_1252 "<?xml version=\"1.0\" encoding=\"Windows-1252\" standalone=\"no\"?><root><a att1=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--><new/></root>"
set expected2_11_iso8859_1 "<?xml version=\"1.0\" encoding=\"ISO8859-1\" standalone=\"no\"?><root><a att2=\"att1\">a content</a><b att1=\"att1\" att2=\"att2\">b content 1</b><b att1=\"att1\" att2=\"att2\">b content 2</b><!--comment--></root>"


#
# Content for container tests
#
global ccontent1
global ccontent2
global ccontent3
global expected_c3
global expected_c1
global expected_c2
global expected_c4
global expected_c5
set ccontent1 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a><c/></a><b>b content 1</b><!--comment--></root>"
set ccontent2 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a/><a/><b/><b/></root>"
set ccontent3 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a><b><c/></b></a><a><b><d dattr=\"foo\"/></b></a></root>"
set expected_c3 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a><b><c/></b></a><a><b><xxx dattr=\"foo\"/></b></a></root>"
set expected_c1 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a attr=\"at\"><c/></a><b>b content 1</b><!--comment--></root>"
set expected_c2 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a attr=\"at\"><c/></a><!--comment--></root>"
set expected_c4 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a attr=\"at\"><c/><new>new content</new></a><!--comment--></root>"
set expected_c5 "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><root><a attr=\"at\"><new1>new content1</new1><c/><new>new content</new></a><!--comment--></root>"


#
# Content for update with results tests
#
global rcontent1
global rcontent2
global rcontent3
global expected_r1
global expected_r2
global expected_r3
global expected_r4
global expected_r5
set rcontent1 "<root><a><c/></a><b>b content 1</b><!--comment--></root>"
set rcontent2 "<root><a><c cattr='a1'/></a><b>b content 1</b><!--comment--></root>"
set expected_r1 "<root><a><c/></a><b>b content 1</b><!--comment--><a><c/></a></root>"
set expected_r2 "<root><a><c/></a><a><c/></a><b>b content 1</b><!--comment--></root>"
set expected_r3 "<root><a><c/></a><b>b content 1</b><d/><!--comment--></root>"
set expected_r4 "<root><a><c/></a><b>b content 1</b><!--comment--><d><a><c/></a></d></root>"
set expected_r5 "<root><a><c><a><c/></a><b>b content 1</b><!--comment--></c></a><b>b content 1</b><!--comment--></root>"

set expected_r6 "<root><b>b content 1</b><a><c/></a><!--comment--></root>"
set expected_r7 "<root><!--comment--><b>b content 1</b><a><c/></a></root>"

