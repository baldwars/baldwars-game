cmake_minimum_required(VERSION 3.19)
<<<<<<< HEAD
project(baldwars-game C)

set(CMAKE_C_STANDARD 99)

add_executable(baldwars-game main.c utils/utils.c utils/utils.h path_finding/path_finding.c path_finding/path_finding.h)
=======
project(BaldWars_Game C)

set(CMAKE_C_STANDARD 11)

include_directories(game)
include_directories(json_file_management)
include_directories(path_finding)
include_directories(utils)

add_executable(BaldWars_Game
        game/game.c
        game/game.h
        game/game_struct.h
        json_file_management/cJSON.c
        json_file_management/cJSON.h
        json_file_management/jsonActionLog.c
        json_file_management/jsonActionLog.h
        path_finding/path_finding.c
        path_finding/path_finding.h
        ai_tools/tools.c
        ai_tools/tools.h
        utils/utils.c
        utils/utils.h
        main.c)
>>>>>>> 8f6ceebeba5ed8a1bf0ce3d834d0d0f588a735e8
