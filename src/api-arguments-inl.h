// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_API_ARGUMENTS_INL_H_
#define V8_API_ARGUMENTS_INL_H_

#include "src/api-arguments.h"

#include "src/tracing/trace-event.h"
#include "src/vm-state-inl.h"

namespace v8 {
namespace internal {

#define SIDE_EFFECT_CHECK(ISOLATE, F, RETURN_TYPE)            \
  do {                                                        \
    if (ISOLATE->needs_side_effect_check() &&                 \
        !PerformSideEffectCheck(ISOLATE, FUNCTION_ADDR(F))) { \
      return Handle<RETURN_TYPE>();                           \
    }                                                         \
  } while (false)

#define FOR_EACH_CALLBACK_TABLE_MAPPING_1_NAME(F)                  \
  F(AccessorNameGetterCallback, "get", v8::Value, Object)          \
  F(GenericNamedPropertyQueryCallback, "has", v8::Integer, Object) \
  F(GenericNamedPropertyDeleterCallback, "delete", v8::Boolean, Object)

#define WRITE_CALL_1_NAME(Function, type, ApiReturn, InternalReturn)          \
  Handle<InternalReturn> PropertyCallbackArguments::Call(Function f,          \
                                                         Handle<Name> name) { \
    Isolate* isolate = this->isolate();                                       \
    SIDE_EFFECT_CHECK(isolate, f, InternalReturn);                            \
    RuntimeCallTimerScope timer(isolate, RuntimeCallCounterId::k##Function);  \
    VMState<EXTERNAL> state(isolate);                                         \
    ExternalCallbackScope call_scope(isolate, FUNCTION_ADDR(f));              \
    PropertyCallbackInfo<ApiReturn> info(begin());                            \
    LOG(isolate,                                                              \
        ApiNamedPropertyAccess("interceptor-named-" type, holder(), *name));  \
    f(v8::Utils::ToLocal(name), info);                                        \
    return GetReturnValue<InternalReturn>(isolate);                           \
  }

FOR_EACH_CALLBACK_TABLE_MAPPING_1_NAME(WRITE_CALL_1_NAME)

#undef FOR_EACH_CALLBACK_TABLE_MAPPING_1_NAME
#undef WRITE_CALL_1_NAME

#define FOR_EACH_CALLBACK_TABLE_MAPPING_1_INDEX(F)            \
  F(IndexedPropertyGetterCallback, "get", v8::Value, Object)  \
  F(IndexedPropertyQueryCallback, "has", v8::Integer, Object) \
  F(IndexedPropertyDeleterCallback, "delete", v8::Boolean, Object)

#define WRITE_CALL_1_INDEX(Function, type, ApiReturn, InternalReturn)        \
  Handle<InternalReturn> PropertyCallbackArguments::Call(Function f,         \
                                                         uint32_t index) {   \
    Isolate* isolate = this->isolate();                                      \
    SIDE_EFFECT_CHECK(isolate, f, InternalReturn);                           \
    RuntimeCallTimerScope timer(isolate, RuntimeCallCounterId::k##Function); \
    VMState<EXTERNAL> state(isolate);                                        \
    ExternalCallbackScope call_scope(isolate, FUNCTION_ADDR(f));             \
    PropertyCallbackInfo<ApiReturn> info(begin());                           \
    LOG(isolate, ApiIndexedPropertyAccess("interceptor-indexed-" type,       \
                                          holder(), index));                 \
    f(index, info);                                                          \
    return GetReturnValue<InternalReturn>(isolate);                          \
  }

FOR_EACH_CALLBACK_TABLE_MAPPING_1_INDEX(WRITE_CALL_1_INDEX)

#undef FOR_EACH_CALLBACK_TABLE_MAPPING_1_INDEX
#undef WRITE_CALL_1_INDEX

Handle<Object> PropertyCallbackArguments::Call(
    GenericNamedPropertySetterCallback f, Handle<Name> name,
    Handle<Object> value) {
  Isolate* isolate = this->isolate();
  SIDE_EFFECT_CHECK(isolate, f, Object);
  RuntimeCallTimerScope timer(
      isolate, RuntimeCallCounterId::kGenericNamedPropertySetterCallback);
  VMState<EXTERNAL> state(isolate);
  ExternalCallbackScope call_scope(isolate, FUNCTION_ADDR(f));
  PropertyCallbackInfo<v8::Value> info(begin());
  LOG(isolate,
      ApiNamedPropertyAccess("interceptor-named-set", holder(), *name));
  f(v8::Utils::ToLocal(name), v8::Utils::ToLocal(value), info);
  return GetReturnValue<Object>(isolate);
}

Handle<Object> PropertyCallbackArguments::Call(
    GenericNamedPropertyDefinerCallback f, Handle<Name> name,
    const v8::PropertyDescriptor& desc) {
  Isolate* isolate = this->isolate();
  SIDE_EFFECT_CHECK(isolate, f, Object);
  RuntimeCallTimerScope timer(
      isolate, RuntimeCallCounterId::kGenericNamedPropertyDefinerCallback);
  VMState<EXTERNAL> state(isolate);
  ExternalCallbackScope call_scope(isolate, FUNCTION_ADDR(f));
  PropertyCallbackInfo<v8::Value> info(begin());
  LOG(isolate,
      ApiNamedPropertyAccess("interceptor-named-define", holder(), *name));
  f(v8::Utils::ToLocal(name), desc, info);
  return GetReturnValue<Object>(isolate);
}

Handle<Object> PropertyCallbackArguments::Call(IndexedPropertySetterCallback f,
                                               uint32_t index,
                                               Handle<Object> value) {
  Isolate* isolate = this->isolate();
  SIDE_EFFECT_CHECK(isolate, f, Object);
  RuntimeCallTimerScope timer(
      isolate, RuntimeCallCounterId::kIndexedPropertySetterCallback);
  VMState<EXTERNAL> state(isolate);
  ExternalCallbackScope call_scope(isolate, FUNCTION_ADDR(f));
  PropertyCallbackInfo<v8::Value> info(begin());
  LOG(isolate,
      ApiIndexedPropertyAccess("interceptor-indexed-set", holder(), index));
  f(index, v8::Utils::ToLocal(value), info);
  return GetReturnValue<Object>(isolate);
}

Handle<Object> PropertyCallbackArguments::Call(
    IndexedPropertyDefinerCallback f, uint32_t index,
    const v8::PropertyDescriptor& desc) {
  Isolate* isolate = this->isolate();
  SIDE_EFFECT_CHECK(isolate, f, Object);
  RuntimeCallTimerScope timer(
      isolate, RuntimeCallCounterId::kIndexedPropertyDefinerCallback);
  VMState<EXTERNAL> state(isolate);
  ExternalCallbackScope call_scope(isolate, FUNCTION_ADDR(f));
  PropertyCallbackInfo<v8::Value> info(begin());
  LOG(isolate,
      ApiIndexedPropertyAccess("interceptor-indexed-define", holder(), index));
  f(index, desc, info);
  return GetReturnValue<Object>(isolate);
}

void PropertyCallbackArguments::Call(AccessorNameSetterCallback f,
                                     Handle<Name> name, Handle<Object> value) {
  Isolate* isolate = this->isolate();
  if (isolate->needs_side_effect_check() &&
      !PerformSideEffectCheck(isolate, FUNCTION_ADDR(f))) {
    return;
  }
  RuntimeCallTimerScope timer(
      isolate, RuntimeCallCounterId::kAccessorNameSetterCallback);
  VMState<EXTERNAL> state(isolate);
  ExternalCallbackScope call_scope(isolate, FUNCTION_ADDR(f));
  PropertyCallbackInfo<void> info(begin());
  LOG(isolate,
      ApiNamedPropertyAccess("interceptor-named-set", holder(), *name));
  f(v8::Utils::ToLocal(name), v8::Utils::ToLocal(value), info);
}

#undef SIDE_EFFECT_CHECK

}  // namespace internal
}  // namespace v8

#endif  // V8_API_ARGUMENTS_INL_H_
