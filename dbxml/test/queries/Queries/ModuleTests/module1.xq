module namespace test = 'http://test.module';

declare function test:function() {
  collection()/dbxml:metadata("dbxml:name")
};
