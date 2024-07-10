#pragma once
#include "testHelper.hpp"

#include <LightAccessory.hpp>
#include <RelayModule.hpp>
#include <ButtonModule.hpp>

TEST_CASE("Test 1", "[LightAccessory] [Constructor] [Stack]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {

        RelayModule relayModule(2);
        ButtonModule buttonModule(5);
        LightAccessory lightAccessory(&relayModule, &buttonModule);

        RelayModule *pRelayModule = new RelayModule(2);
        ButtonModule *pButtonModule = new ButtonModule(5);

        LightAccessory lightAccessory1(pRelayModule, pButtonModule);

        delete pRelayModule;
        delete pButtonModule;

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}

TEST_CASE("Test 2","[LightAccessory] [Constructor] [Heap]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {
        RelayModule *pRelayModule = new RelayModule(2);
        ButtonModule *pButtonModule = new ButtonModule(5);

        LightAccessory *pLightAccessory = new LightAccessory(pRelayModule, pButtonModule);

        delete pLightAccessory;
        delete pRelayModule;
        delete pButtonModule;

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}

TEST_CASE("Test 3","[LightAccessory] [setPowerState] [Stack]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {
        RelayModule relayModule(2,1,0);
        ButtonModule buttonModule(5);
        LightAccessory lightAccessory(&relayModule, &buttonModule);

        TEST_ASSERT_FALSE(relayModule.isOn());

        lightAccessory.setPowerState(true);
        TEST_ASSERT_TRUE(relayModule.isOn()); 

        lightAccessory.setPowerState(false);
        TEST_ASSERT_FALSE(relayModule.isOn());

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}

TEST_CASE("Test 4","[LightAccessory] [setPowerState] [Heap]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {
        RelayModule *pRelayModule = new RelayModule(2,1,0);
        ButtonModule *pButtonModule = new ButtonModule(5);
        LightAccessory *pLightAccessory = new LightAccessory(pRelayModule, pButtonModule);

        TEST_ASSERT_FALSE(pRelayModule->isOn());

        pLightAccessory->setPowerState(true);
        TEST_ASSERT_TRUE(pRelayModule->isOn()); 

        pLightAccessory->setPowerState(false);
        TEST_ASSERT_FALSE(pRelayModule->isOn());

        delete pLightAccessory;
        delete pRelayModule;
        delete pButtonModule;

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}

TEST_CASE("Test 5","[LightAccessory] [getPowerState] [Stack]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {
        RelayModule relayModule(2,1,0);
        ButtonModule buttonModule(5);
        LightAccessory lightAccessory(&relayModule, &buttonModule);

        TEST_ASSERT_EQUAL(relayModule.isOn(), lightAccessory.isPowerOn());

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}

TEST_CASE("Test 6","[LightAccessory] [getPowerState] [Heap]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {
        RelayModule *pRelayModule = new RelayModule(2,1,0);
        ButtonModule *pButtonModule = new ButtonModule(5);
        LightAccessory *pLightAccessory = new LightAccessory(pRelayModule, pButtonModule);

        TEST_ASSERT_EQUAL(pRelayModule->isOn(), pLightAccessory->isPowerOn());

        delete pLightAccessory;
        delete pRelayModule;
        delete pButtonModule;

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}

TEST_CASE("Test 7","[LightAccessory] [setReportCallback] [Stack]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {
        RelayModule relayModule(2,1,0);
        ButtonModule buttonModule(5);
        LightAccessory lightAccessory(&relayModule, &buttonModule);

        lightAccessory.setReportCallback([](void* aaa, bool bbb){}, nullptr);

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}

TEST_CASE("Test 8","[LightAccessory] [setReportCallback] [Heap]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {
        RelayModule *pRelayModule = new RelayModule(2,1,0);
        ButtonModule *pButtonModule = new ButtonModule(5);
        LightAccessory *pLightAccessory = new LightAccessory(pRelayModule, pButtonModule);

        pLightAccessory->setReportCallback([](void* aaa, bool bbb){}, nullptr);

        delete pLightAccessory;
        delete pRelayModule;
        delete pButtonModule;

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}

TEST_CASE("Test 9","[LightAccessory] [identify] [Stack]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {
        RelayModule relayModule(2,1,0);
        ButtonModule buttonModule(5);
        LightAccessory lightAccessory(&relayModule, &buttonModule);

        lightAccessory.identify();

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}

TEST_CASE("Test 10","[LightAccessory] [identify] [Heap]")
{
    heap_trace_record_t trace_record[10];
    BEGIN_MEMORY_LEAK_TEST(trace_record);
    do
    {
        RelayModule *pRelayModule = new RelayModule(2,1,0);
        ButtonModule *pButtonModule = new ButtonModule(5);
        LightAccessory *pLightAccessory = new LightAccessory(pRelayModule, pButtonModule);

        pLightAccessory->identify();

        vTaskDelay(3000 / portTICK_PERIOD_MS);

        delete pLightAccessory;
        delete pRelayModule;
        delete pButtonModule;

    } while (0);
    END_MEMORY_LEAK_TEST(trace_record);
}