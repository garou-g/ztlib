#include "unity.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "module.hpp"

class ModuleExt final : public Module {
public:
    ModuleExt() { }

    void publicSetAvailability(bool value)
    {
        setAvailability(value);
    }

protected:
    Time _dispatcher() override
    {
        return Time(0, 1, 0);
    }
};

TEST_CASE("constructor", "[module]")
{
    Module* mod = nullptr;
    TEST_ASSERT(mod == nullptr);

    mod = new ModuleExt();

    TEST_ASSERT(mod != nullptr);
    TEST_ASSERT(mod->isInited());
    TEST_ASSERT(mod->nextCallTime() == 0);
    TEST_ASSERT(!mod->isSuspended());
    TEST_ASSERT(mod->isAvailable());

    delete mod;
}

TEST_CASE("isAvailable", "[module]")
{
    ModuleExt* mod = new ModuleExt();

    mod->publicSetAvailability(true);
    TEST_ASSERT(mod->isAvailable());

    delete mod;
    mod = new ModuleExt();

    mod->publicSetAvailability(false);
    TEST_ASSERT(!mod->isAvailable());

    delete mod;
}

TEST_CASE("dispatcher", "[module]")
{
    Module* mod = new ModuleExt();

    TEST_ASSERT(mod->nextCallTime() == 0);
    TEST_ASSERT(mod->delayTime() == 0);

    Time nextCallTime = Time::now() + Time(0, 1, 0);
    Time delayTime =  Time(0, 1, 0);

    mod->dispatcher();
    TEST_ASSERT(mod->nextCallTime() > (nextCallTime - 50)
        && mod->nextCallTime() < (nextCallTime + 50));
    TEST_ASSERT(mod->delayTime() > (delayTime - 50)
        && mod->delayTime() < (delayTime + 50));

    int32_t delayMs = 100;
    vTaskDelay(pdMS_TO_TICKS(delayMs));
    delayTime -= delayMs;
    TEST_ASSERT(mod->nextCallTime() > (nextCallTime - 50)
        && mod->nextCallTime() < (nextCallTime + 50));
    TEST_ASSERT(mod->delayTime() > (delayTime - 50)
        && mod->delayTime() < (delayTime + 50));

    delayMs = 1000;
    vTaskDelay(pdMS_TO_TICKS(delayMs));
    TEST_ASSERT(mod->nextCallTime() > (nextCallTime - 50)
        && mod->nextCallTime() < (nextCallTime + 50));
    TEST_ASSERT(mod->delayTime() == 0);

    delete mod;
}
