// Copyright 2014 MongoDB Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <tuple>

#include <bsoncxx/config/prelude.hpp>

namespace bsoncxx {
BSONCXX_INLINE_NAMESPACE_BEGIN
namespace builder {
namespace basic {

///
/// Constructs a key-value pair from a string key
/// and a BSON value.
///
template <typename T, typename U>
BSONCXX_INLINE std::tuple<T&&, U&&> kvp(T&& t, U&& u) {
    return std::tuple<T&&, U&&>(std::forward<T>(t), std::forward<U>(u));
}

}  // namespace basic
}  // namespace builder
BSONCXX_INLINE_NAMESPACE_END
}  // namespace bsoncxx

#include <bsoncxx/config/postlude.hpp>
