#include "grace/descriptor_set_layout.hpp"

#include <doctest/doctest.h>

#include "test_utils.hpp"

using namespace grace;

TEST_SUITE("DescriptorSetLayouts")
{
  TEST_CASE("make_descriptor_set_layout_binding")
  {
    const uint32 index = 12;
    const auto type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    const auto stages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    const uint32 count = 4;

    const auto binding = make_descriptor_set_layout_binding(index, type, stages, count);

    CHECK(binding.binding == index);
    CHECK(binding.descriptorType == type);
    CHECK(binding.stageFlags == stages);
    CHECK(binding.descriptorCount == count);
    CHECK(binding.pImmutableSamplers == nullptr);
  }

  TEST_CASE("make_descriptor_set_layout_binding_flags_info")
  {
    const auto flag = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT;
    const std::vector<VkDescriptorBindingFlags> flags(4, flag);

    const auto flags_info = make_descriptor_set_layout_binding_flags_info(flags);

    CHECK(flags_info.sType ==
          VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO);
    CHECK(flags_info.pNext == nullptr);
    CHECK(flags_info.pBindingFlags == flags.data());
    CHECK(flags_info.bindingCount == u32_size(flags));
  }

  TEST_CASE("make_descriptor_set_layout_info")
  {
    const std::vector<VkDescriptorSetLayoutBinding> bindings = {
        {
            .binding = 42,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };

    const auto layout_info = make_descriptor_set_layout_info(bindings);

    CHECK(layout_info.sType == VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO);
    CHECK(layout_info.flags == 0);
    CHECK(layout_info.pNext == nullptr);
    CHECK(layout_info.pBindings == bindings.data());
    CHECK(layout_info.bindingCount == u32_size(bindings));
  }

  TEST_CASE("DescriptorSetLayout::ctor")
  {
    DescriptorSetLayout layout;
    CHECK(!layout);
    CHECK(layout.get() == VK_NULL_HANDLE);
    CHECK(layout.device() == VK_NULL_HANDLE);
    CHECK_NOTHROW(layout.destroy());
  }

  TEST_CASE("DescriptorSetLayout::make")
  {
    auto ctx = make_test_context();

    const std::vector<VkDescriptorSetLayoutBinding> bindings = {};
    const auto layout_info = make_descriptor_set_layout_info(bindings);

    VkResult result = VK_ERROR_UNKNOWN;
    auto layout = DescriptorSetLayout::make(ctx.device, layout_info, &result);

    REQUIRE(result == VK_SUCCESS);
    CHECK(layout);
    CHECK(layout.get() != VK_NULL_HANDLE);
    CHECK(layout.device() != VK_NULL_HANDLE);

    layout.destroy();
    CHECK(!layout);
    CHECK(layout.get() == VK_NULL_HANDLE);
    CHECK(layout.device() != VK_NULL_HANDLE);
  }

  TEST_CASE("DescriptorSetLayoutBuilder::build")
  {
    auto ctx = make_test_context();
    DescriptorSetLayoutBuilder builder {ctx.device};

    VkResult result = VK_ERROR_UNKNOWN;
    auto layout = builder.reset()
                      .use_push_descriptors()
                      .allow_partially_bound_descriptors()
                      .descriptor(0,
                                  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                  VK_SHADER_STAGE_FRAGMENT_BIT)
                      .descriptor(1,
                                  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                  VK_SHADER_STAGE_FRAGMENT_BIT)
                      .build(&result);

    REQUIRE(result == VK_SUCCESS);
    CHECK(layout);
    CHECK(layout.get() != VK_NULL_HANDLE);
    CHECK(layout.device() == ctx.device.get());
  }
}