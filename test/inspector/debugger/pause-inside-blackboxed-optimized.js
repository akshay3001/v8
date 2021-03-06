// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Flags: --allow-natives-syntax

let {session, contextGroup, Protocol} = InspectorTest.start(
  'Checks pause inside blackboxed optimized function call.');

contextGroup.addScript(`
  function foo() {
    return 1 + bar();
  }
  //# sourceURL=framework.js
`);

contextGroup.addScript(`
  function bar() {
    return 2;
  }
  foo();
  foo();
  %OptimizeFunctionOnNextCall(foo);
  foo();
  //# sourceURL=test.js
`);

(async function test(){
  Protocol.Debugger.enable();
  Protocol.Debugger.setBlackboxPatterns({patterns: ['framework\.js']});
  Protocol.Runtime.evaluate({expression: `
  inspector.callWithScheduledBreak(foo, 'break', '');
  //# sourceURL=expr.js
  `});
  const {params:{callFrames}} = await Protocol.Debugger.oncePaused();
  session.logCallFrames(callFrames);
  InspectorTest.completeTest();
})();
