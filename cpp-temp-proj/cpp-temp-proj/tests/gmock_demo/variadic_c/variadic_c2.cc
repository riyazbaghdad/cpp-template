/**
 * A demo test to showcase mocking variadic C functions in Google Mock.
 * By Ari Saif
 */
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
using ::testing::_;

class PanicWrapperInterface {
 public:
  virtual void DummyPanic() = 0;
};

// Production wrapper.
class PanicWrapper : public PanicWrapperInterface {
 public:
  void DummyPanic() {}
};

// Mock wrapper.
class MockPanicWrapper : public PanicWrapperInterface {
 public:
  MOCK_METHOD(void, DummyPanic, (), ());
};

// Variadic function.
// The panic function should take an extra parameter mock_interface.
// Alternatively, the extra parameter can be converted to a global variable.
extern "C" {
void panic(PanicWrapperInterface *mock_interface, const char *fmt, ...) {
  if (dynamic_cast<PanicWrapper *>(mock_interface)) {
    // The production implementation of panic function. Used only for
    // production, not for testing.
    va_list v;
    va_start(v, fmt);
    vprintf(fmt, v);
    va_end(v);
    putchar('\n');
  } else {
    mock_interface->DummyPanic();
  }
}
}

// The foo function should take an extra parameter mock_interface.
// Alternatively, the extra parameter can be converted to a global variable.
void foo(PanicWrapperInterface *mock_interface, bool some_condition) {
  if (some_condition) {
    panic(mock_interface, "error message");
  }
}

TEST(Panic, isCalled) {
  MockPanicWrapper mock_interface;

  EXPECT_CALL(mock_interface, DummyPanic()).Times(1);
  foo(&mock_interface, /*some_condition=*/true);
}

TEST(Panic, isNotCalled) {
  MockPanicWrapper mock_interface;

  EXPECT_CALL(mock_interface, DummyPanic()).Times(0);
  foo(&mock_interface, /*some_condition=*/false);
}

TEST(Panic, productionWrapperWorksCorrectly) {
  // Use the production wrapper
  PanicWrapper panic_wrapper;
  testing::internal::CaptureStdout();

  // This should print "error message" in the output.
  foo(&panic_wrapper, /*some_condition=*/true);
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "error message\n");
}