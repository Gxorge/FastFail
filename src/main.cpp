#include "main.hpp"

#include "GlobalNamespace/StandardLevelFailedController.hpp"
#include "GlobalNamespace/StandardLevelFailedController_InitData.hpp"
#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "GlobalNamespace/PrepareLevelCompletionResults.hpp"
#include "GlobalNamespace/MissionLevelFailedController.hpp"
#include "GlobalNamespace/MissionLevelFailedController_InitData.hpp"
#include "GlobalNamespace/MissionObjectiveResult.hpp"
#include "GlobalNamespace/MissionCompletionResults.hpp"
#include "GlobalNamespace/MissionObjectiveCheckersManager.hpp"
#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"
using namespace GlobalNamespace;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

MAKE_HOOK_OFFSETLESS(StandardLevelFailed, void, StandardLevelFailedController* self) {
    StandardLevelFailed(self);

    getLogger().info("User failed standard!");

    self->StopAllCoroutines();
    StandardLevelFailedController::InitData* initData = self->initData;
    PrepareLevelCompletionResults* prep = self->prepareLevelCompletionResults;
    StandardLevelScenesTransitionSetupDataSO* setupData = self->standardLevelSceneSetupData;

    LevelCompletionResults::LevelEndAction action = (initData->autoRestart ? LevelCompletionResults::LevelEndAction::_get_Restart() : LevelCompletionResults::LevelEndAction::_get_None());
    LevelCompletionResults* results = prep->FillLevelCompletionResults(LevelCompletionResults::LevelEndStateType::_get_Failed(), action);
    setupData->Finish(results);
}

MAKE_HOOK_OFFSETLESS(MissionLevelFailed, void, MissionLevelFailedController* self) {
    MissionLevelFailed(self);

    getLogger().info("User failed mission!");

    self->StopAllCoroutines();
    MissionLevelFailedController::InitData* initData = self->initData;
    PrepareLevelCompletionResults* prep = self->prepareLevelCompletionResults;
    MissionLevelScenesTransitionSetupDataSO* setupData = self->missionLevelSceneSetupData;
    MissionObjectiveCheckersManager* checkersManager = self->missionObjectiveCheckersManager;

    LevelCompletionResults::LevelEndAction action = (initData->autoRestart ? LevelCompletionResults::LevelEndAction::_get_Restart() : LevelCompletionResults::LevelEndAction::_get_None());
    LevelCompletionResults* results = prep->FillLevelCompletionResults(LevelCompletionResults::LevelEndStateType::_get_Failed(), action);

    Array<MissionObjectiveResult*>* missionResults = checkersManager->GetResults();
    MissionCompletionResults* completionResults = new MissionCompletionResults(results, missionResults);

    setupData->Finish(completionResults);
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");
    
    INSTALL_HOOK_OFFSETLESS(getLogger(), StandardLevelFailed, il2cpp_utils::FindMethodUnsafe("", "StandardLevelFailedController", "HandleLevelFailed", 0));
    //INSTALL_HOOK_OFFSETLESS(getLogger(), MissionLevelFailed, il2cpp_utils::FindMethodUnsafe("", "MissionLevelFailedController", "HandleLevelFailed", 0)); TODO: fix crash

    getLogger().info("Installed all hooks!");
}