/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "DexClass.h"

namespace api {

struct MRefInfo {
  DexMethodRef* mref;
  DexAccessFlags access_flags;

  MRefInfo(DexMethodRef* _mref, DexAccessFlags _access_flags)
      : mref(_mref), access_flags(_access_flags) {}
};

struct FRefInfo {
  DexFieldRef* fref;
  DexAccessFlags access_flags;

  FRefInfo(DexFieldRef* _fref, DexAccessFlags _access_flags)
      : fref(_fref), access_flags(_access_flags) {}
};

struct FrameworkAPI {
  DexType* cls;
  DexType* super_cls;
  std::vector<MRefInfo> mrefs_info;
  std::vector<FRefInfo> frefs_info;
  DexAccessFlags access_flags;
};

using TypeToFrameworkAPI = std::unordered_map<const DexType*, FrameworkAPI>;

class ApiLevelsUtils {
 public:
  ApiLevelsUtils(const Scope& scope,
                 const std::string& framework_api_info_filename,
                 int api_level)
      : m_framework_api_info_filename(framework_api_info_filename),
        m_api_level(api_level) {
    // Setting up both m_types_to_framework_api and m_framework_classes
    load_framework_api(scope);
  }

  const TypeToFrameworkAPI& get_types_to_framework_api() {
    return m_types_to_framework_api;
  }

  std::unordered_map<DexType*, FrameworkAPI> get_framework_classes();

  /**
   * NOTE: Workaround for the fact that real private members can be made public
   * by any pass ... We gather:
   *  - members that are accessed outside of their own class
   *  - true virtual methods
   *
   * NOTE: This needs to run every time something changes in the scope.
   */
  void gather_non_private_members(const Scope& scope);

  void filter_types(const std::unordered_set<const DexType*>& types,
                    const Scope& scope);

 private:
  void load_framework_api(const Scope& scope);
  void check_and_update_release_to_framework(const Scope& scope);

  TypeToFrameworkAPI m_types_to_framework_api;
  std::unordered_set<DexType*> m_framework_classes;
  std::string m_framework_api_info_filename;
  uint32_t m_api_level;

  /**
   * NOTE: Those work as "non-private" in the sense that we check where
   *       they are referenced:
   */
  std::unordered_set<DexMethodRef*> m_methods_non_private;
  std::unordered_set<DexFieldRef*> m_fields_non_private;
};

} // namespace api
