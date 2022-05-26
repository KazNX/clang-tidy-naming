//
// CSIRO Open Source Software Licence Agreement (variation of the BSD / MIT License)
// Copyright (c) 2020, Commonwealth Scientific and Industrial Research Organisation (CSIRO) ABN 41 687 119 230.
//
// All rights reserved. CSIRO is willing to grant you a licence to this file on the following terms, except where
// otherwise indicated for third party material. Redistribution and use of this software in source and binary forms,
// with or without modification, are permitted provided that the following conditions are met:
// - Redistributions of source code must retain the above copyright notice, this list of conditions and the following
//   disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
// the following disclaimer in the documentation and/or other materials provided with the distribution.
// - Neither the name of CSIRO nor the names of its contributors may be used to endorse or promote products derived from
// this software without specific prior written permission of CSIRO.
//
// EXCEPT AS EXPRESSLY STATED IN THIS AGREEMENT AND TO THE FULL EXTENT PERMITTED BY APPLICABLE LAW, THE SOFTWARE IS
// PROVIDED "AS-IS". CSIRO MAKES NO REPRESENTATIONS, WARRANTIES OR CONDITIONS OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS REGARDING THE CONTENTS OR ACCURACY OF THE SOFTWARE,
// OR OF TITLE, MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT, THE ABSENCE OF LATENT OR OTHER
// DEFECTS, OR THE PRESENCE OR ABSENCE OF ERRORS, WHETHER OR NOT DISCOVERABLE. TO THE FULL EXTENT PERMITTED BY
// APPLICABLE LAW, IN NO EVENT SHALL CSIRO BE LIABLE ON ANY LEGAL THEORY (INCLUDING, WITHOUT LIMITATION, IN AN ACTION
// FOR BREACH OF CONTRACT, NEGLIGENCE OR OTHERWISE) FOR ANY CLAIM, LOSS, DAMAGES OR OTHER LIABILITY HOWSOEVER INCURRED.
// WITHOUT LIMITING THE SCOPE OF THE PREVIOUS SENTENCE THE EXCLUSION OF LIABILITY SHALL INCLUDE: LOSS OF PRODUCTION OR
// OPERATION TIME, LOSS, DAMAGE OR CORRUPTION OF DATA OR RECORDS; OR LOSS OF ANTICIPATED SAVINGS, OPPORTUNITY, REVENUE,
// PROFIT OR GOODWILL, OR OTHER ECONOMIC LOSS; OR ANY SPECIAL, INCIDENTAL, INDIRECT, CONSEQUENTIAL, PUNITIVE OR
// EXEMPLARY DAMAGES, ARISING OUT OF OR IN CONNECTION WITH THIS AGREEMENT, ACCESS
//
// Author: Kazys Stepanas

/// @file Naming.cpp
/// This file shows how clang-tidy categorises identifiers for naming purposes and the order of category resolution.
///
/// Clang tidy has many categorisations for the @c readability-identifier-naming check, to support enforcing custom code
/// styles. Many of these categorisations overlap, with an individual identifier falling into several categorisations.
/// These categorisations are resolved in an order determined by the code for @c readability-identifier-naming ,
/// specifically in the function @c clang::tidy::IdentifierNamingCheck::findStyleKind() . The documentation for
/// clang-tidy is sometimes unclear on the semantics of a particular categorisation label and does not document the
/// resolution order for these categories.
///
/// This file seeks to illustrate the resolution order using doxygen style comments (except for local variables - see
/// @c varFunc() ). Each identifier has doxygen documentation comments which specifies all the categories to which that
/// identifier belongs, listed in resolution order.
///
/// Bellow is a summary of the categorisations clang-tidy supports. The list below has been taken from the clang-tidy
/// source and shows the category resolution order. This order is estimated to be value from at least clang-tidy version
/// 10 through 15, with the exception that @c ScopedEnumValue became available from version 12. Some items will appear
/// multiple times as they can be checked in different context. Clang tidy will return the first valid categorisation is
/// matched. This occures when the semantics of the identifier match and there is a valid entry for the categorisation
/// present in the @c .clang-tidy file.
///
/// Labels listed in @c <> brackets are semantic qualifiers and attempt to illustrate the semantic context within which
/// clang-tidy resolves the categorisation. There are not formal semantic labels, rather labels which attempt
/// disambiguation within the context of this document. For example, @c <member-variables> identifiers that clang tidy
/// is currently looking only at member variables.
///
/// Items in @c [] brackets provide C/C++ keywords relevant to that particular categorisation. These must be present in
/// the declaration for clang-tidy to match the category.
///
/// Nested items are grouped within a @c <semantic-context> - literally mapping to a C++ scope in the
/// @c clang::tidy::IdentifierNamingCheck::findStyleKind() function.
///
/// The label @c Invalid essentially indicates that the categorisation function has a return statement in that scope
/// and clang-tidy will discontinue further categorisation attempts for the current item. For example, in the
/// @c <paramaters> semantic context, clang tidy will abort matching after failing to resolve the @c Parameter
/// categorisation and a parameter will not be categorised as a @c Variable .
///
/// The check resolution order is listed below. The category labels below can be used in a @c .clang-tidy file in the
/// form shown below:
/// @code{.yaml}
/// Checks: >
///   readability-identifier-naming
/// CheckOptions:
///   - key: readability-identifier-naming.<Name>[Case|Prefix|IgnoredRegexp|Suffix]
///     value: [aNy_CasE|lower_case|UPPER_CASE|camelBack|CamelCase|Camel_Snake_Case|camel_Snake_Back]
/// @endcode
///
/// - Typedef  [typedef]
/// - TypeAlias  [using Alias = ...]
/// - InlineNamespace [inline namespace]
/// - Namespace [namespace]
/// - <enum-members>
///   - ScopedEnumValue [enum class member]
///   - EnumConstant  [enum member]
///   - Constant
///   - Invalid
/// - <user-record-types>
///   - AbstractClass [class, struct, pure-virtual present]
///   - Struct [struct]
///   - Class [class, struct]
///   - Struct [class]
///   - Union [union]
///   - Enum [enum, enum class]
///   - Invalid
/// - <member-variables> - does not cover [static, constexpr]
///   - [const]
///     - ConstantMember
///     - Constant
///   - PrivateMember [private]
///   - ProtectedMember [protected]
///   - PublicMember [public]
///   - Member
///   - Invalid
/// - <parameters>
///   - ConstexprVariable [constexpr]
///   - [const]
///     - ConstantPointerParameter [*]
///     - ConstantParameter
///     - Constant
///   - ParameterPack [...]
///   - PointerParameter [*]
///   - Parameter
///   - Invalid
/// - <variable>
///   - ConstexprVariable [constexpr]
///   - [const]
///     - ClassConstant [const, static]
///     - <file-level-variable>
///       - GlobalConstantPointer [const *]
///       - GlobalConstant [const]
///     - StaticConstant [static, const]
///     - <local-variable>
///       - LocalConstantPointer [const *]
///       - LocalConstant [const]
///     - Constant [const]
///   - <class-level>
///     - ClassMember [static]
///   - <file-level-variable>
///     - GlobalPointer [*]
///     - GlobalVariable
///   - <local-variable>
///     - StaticVariable [static]
///     - LocalPointer [*]
///     - LocalVariable
///   - <function/method-local-variable>
///     - LocalVariable
///   - Variable
/// - <class method>
///   - <ignore-base-class-method-overrides>
///   - [constexpr]
///     - ConstexprMethod
///     - ConstexprFunction
///   - ClassMethod [static]
///   - VirtualMethod [virtual]
///   - PrivateMethod [private]
///   - ProtectedMethod [protected]
///   - PublicMethod [public]
///   - Method
///   - Function
///   - Invalid
/// - <functions>
///   - <ignore main()>
///   - ConstexprFunction [constexpr]
///   - GlobalFunction [static method, static function, in any namespace]
///   - Function
///   - Invalid
/// - <template parameter>
///   - <template-type-parameter>
///     - TypeTemplateParameter
///     - TemplateParameter
///     - Invalid
///   - <template-non-type-parameter>
///     - ValueTemplateParameter
///     - TemplateParameter
///     - Invalid
///   - <template-template-parameter> (?)
///     - TemplateTemplateParameter
///     - TemplateParameter
///     - Invalid
/// - Invalid

#include <array>
#include <functional>
#include <iostream>
#include <string>

/// MacroDefinition
#define MACRO_DEFINITION 42
/// MacroDefinition
#define MACRO_FUNCTION(x) (void)x

/// Typedef
typedef int MyArchaicInt;
/// TypeAlias
using MyContemporaryInt = int;

/// Namespace
namespace some_namespace
{
/// InlineNamespace, Namespace
inline namespace version_1
{
}  // namespace version_1
}  // namespace some_namespace

/// AbstractClass, Class, Struct
class AbstractClass
{
public:
  /// Not checked - constructors and destructors must match class name.
  inline AbstractClass() = default;
  /// Not checked - constructors and destructors must match class name.
  inline virtual ~AbstractClass() {}

  /// PublicMethod, Method, Function
  void notAbstract();

  /// VirtualMethod, PublicMethod, Method, Function
  virtual void abstractMethod() = 0;
};

/// Class, Struct
class ClassNaming
{
  // ----------------------
  // Class member variables
public:
  /// ConstexprVariable, ClassConstant, Constant, ClassMember, GlobalConstant, GlobalVariable, Variable
  constexpr static int public_constexpr_member = 0;

  /// ConstantMember, Constant, PublicMember, Member
  const int public_constant_member = 0;

  /// PublicMember, Member
  int public_member = 0;

  /// ClassMember, Member, Variable
  static int public_static_member;

  /// ClassConstant, Constant, GlobalConstant, GlobalVariable, Variable
  static const int public_static_constant_member = 0;

protected:
  /// ConstexprVariable, ClassConstant, Constant, ClassMember, GlobalConstant, GlobalVariable, Variable
  constexpr static int protected_constexpr_member = 0;

  /// ConstantMember, Constant, ProtectedMember, Member
  const int protected_constant_member = 0;

  /// ProtectedMember, Member
  int protected_member_ = 0;

  /// ClassMember, Member, Variable
  static int protected_static_member;

  /// ClassConstant, Constant, GlobalConstant, GlobalVariable, Variable
  static const int protected_static_constant_member = 0;

private:
  /// ConstexprVariable, ClassConstant, Constant, ClassMember, GlobalConstant, GlobalVariable, Variable
  constexpr static int private_constexpr_member = 0;

  /// ConstantMember, Constant, PrivateMember, Member
  const int private_constant_member_ = 0;

  /// PrivateMember, Member
  int private_member_ = 0;

  /// ClassMember, Member, Variable
  static int private_static_member;

  /// ClassConstant, Constant, GlobalConstant, GlobalVariable, Variable
  static const int private_static_constant_member = 0;

  // ----------------------
  // Class methods
public:
  /// ConstexprMethod, ConstexprFunction, PublicMethod, Method, Function
  constexpr int publicConstexprFunc() { return 0; }

  /// ClassMethod, PublicMethod, Method, Function
  static int publicStaticFunc() { return 0; }

  /// VirtualMethod, PublicMethod, Method, Function
  virtual int publicFunc() { return 0; }

protected:
  /// ConstexprMethod, ConstexprFunction, ProtectedMethod, Method, Function
  constexpr int protectedConstexprFunc() { return 0; }

  /// ClassMethod, ProtectedMethod, Method, Function
  static int protectedStaticFunc() { return 0; }

  /// VirtualMethod, ProtectedMethod, Method, Function
  virtual int protectedFunc() { return 0; }

private:
  /// ConstexprMethod, ConstexprFunction, PrivateMethod, Method, Function
  constexpr int privateConstexprFunc() { return 0; }

  /// ClassMethod, PrivateMethod, Method, Function
  static int privateStaticFunc() { return 0; }

  /// VirtualMethod, PrivateMethod, Method, Function
  virtual int privateFunc() { return 0; }
};

int ClassNaming::public_static_member = 0;
int ClassNaming::protected_static_member = 0;
int ClassNaming::private_static_member = 0;

/// Struct, Class
struct StructNaming
{
};

/// Union
union UnionNaming
{
  // PublicMember, Member
  int union_variable = 0;
};

/// Enum
enum class EnumClass
{
  // ScopedEnumConstant, EnumConstant, Constant
  EnumClassValue
};

/// Enum
enum OldEnum
{
  // EnumConstant, Constant
  OLD_ENUM_VALUE
};

/// GlobalFunction, Function
/// @param str_ptr ConstantPointerParameter, ConstantParameter, PointerParameter, Parameter
/// @param str ConstantParameter, Parameter
/// @param ptr_param PointerParameter, Parameter
/// @param param Parameter
int func(std::string *const str_ptr, const std::string &str, int *ptr_param, int param)
{
  return 0;
}

// ---------
// Templates
// ---------

/// Class, Struct
/// @tparam Type TypeTemplateParameter, TemplateParameter
/// @tparam Size ValueTemplateParameter, TemplateParameter
template <typename Type, int Size>
class ArrayTemplate
{
public:
  /// PublicMember, Member
  Type bytes[Size];
};

/// GlobalFunction, Function
/// @tparam Param TypeTemplateParameter, TemplateParameter
/// @param val Parameter
template <typename Param>
void templatePrint(const Param &val)
{
  std::cout << val;
}

/// GlobalFunction, Function
/// @tparam Param TypeTemplateParameter, TemplateParameter
/// @tparam Args TypeTemplateParameter, TemplateParameter
/// @param val Parameter
/// @param args ParameterPack, Parameter
template <typename Param, typename... Args>
void templatePrint(const Param &val, Args... args)
{
  templatePrint(val);
  std::cout << ' ';
  templatePrint(args...);
}

/// GlobalFunction, Function
/// @tparam Callable TemplateTemplateParameter, TemplateParameter
/// @tparam Param TypeTemplateParameter, TemplateParameter
/// @param callable Parameter
/// @param arg Parameter
template <template <typename> class Callable, typename Param>
Param templateTemplateParam(const Callable<Param(const Param &)> &callable, const Param &arg)
{
  return callable(arg);
}

// -------------------------------
// Non-class variable declarations
// -------------------------------

// File level variables (global)
//
// The categorisation of anonymous namespace and named namespace variables is the same as for the global variable
// declarations below.

/// ConstexprVariable, GlobalConstant, Constant, GlobalVariable, Variable
constexpr int global_constexpr = 0;

/// GlobalConstantPointer, GlobalConstant, Constant, GlobalPointer, GlobalVariable, Variable
int *const global_const_ptr = nullptr;

/// GlobalPointer, GlobalVariable, Variable
const int *global_const_int_ptr = nullptr;

/// GlobalConstant, Constant, GlobalVariable, Variable
const int global_const = 0;

/// GlobalConstant, [StaticConstant,] Constant, GlobalVariable, Variable
///
/// StaticConstant does not actually trip for this declaration despite the documentation indicating that it should.
/// StaticConstant does not appear to trip for anything. Reading the code, it seems that StaticConstant logic is in the
/// wrong place and the conditions cannot be met.
static const int global_static_const = 0;

/// GlobalConstantPointer, GlobalConstant, [StaticConstant,] Constant, GlobalPointer, GlobalVariable, Variable
///
/// See @c global_static_const regarding StaticConstant
static int *const global_static_const_ptr = 0;

/// GlobalPointer, GlobalVariable, Variable
static const int *global_static_const_int_ptr = 0;

/// GlobalFunction, Function
///
/// @param seed Parameter
int varFunc(int seed = 0)
{
  // ConstexprVariable, LocalConstant, Constant, LocalVariable, Variable
  constexpr int local_constexpr = 0;

  // LocalConstant, Constant, LocalVariable, Variable
  const int local_const = 0;

  // LocalConstantPointer, LocalConstant, Constant, LocalPointer, LocalVariable, Variable
  const int *const local_const_ptr = &local_const;

  // LocalConstant, Constant, LocalVariable, Variable
  const auto local_lambda = [](int value) { return value; };

  // StaticVariable, LocalVariable, Variable
  static int static_variable = 0;

  // LocalVariable, Variable
  int local_variable = seed;

  // LocalPointer, LocalVariable, Variable
  int *local_variable_ptr = &local_variable;

  return local_lambda(local_constexpr + local_const + *local_const_ptr + static_variable + local_variable +
                      *local_variable_ptr);
}

// -------------------------------
// Non-class function declarations
// -------------------------------

/// ConstexprFunction, GlobalFunction, Function
constexpr int answer()
{
  return 42;
}

/// GlobalFunction, Function
static int staticFunc()
{
  return 0;
}

namespace
{
/// Function
///
/// Not a GlobalFunction because it's in a namespace.
int anonymousFunc()
{
  return 0;
}
}  // namespace

/// GlobalFunction, Function
int otherFunc()
{
  return 0;
}
