# GLOB 不包含子目录 GLOB_RECURSE 包含子目录
FILE(GLOB head_core core/*.h)
FILE(GLOB source_core core/*.cpp)
SET(group_core ${head_core} ${source_core})
SOURCE_GROUP(core FILES ${group_core})
FILE(GLOB head_core_algorithm core/algorithm/*.h)
FILE(GLOB source_core_algorithm core/algorithm/*.cpp)
SET(group_core_algorithm ${head_core_algorithm} ${source_core_algorithm})
SOURCE_GROUP(core/algorithm FILES ${group_core_algorithm})
FILE(GLOB head_core_problem core/problem/*.h)
FILE(GLOB source_core_problem core/problem/*.cpp)
SET(group_core_problem ${head_core_problem} ${source_core_problem})
SOURCE_GROUP(core/problem FILES ${group_core_problem})
FILE(GLOB head_core_problem_continuous core/problem/continuous/*.h)
FILE(GLOB source_core_problem_continuous core/problem/continuous/*.cpp)
SET(group_core_problem_continuous ${head_core_problem_continuous} ${source_core_problem_continuous})
SOURCE_GROUP(core/problem/continuous FILES ${group_core_problem_continuous})
FILE(GLOB head_core_record core/record/*.h)
FILE(GLOB source_core_record core/record/*.cpp)
SET(group_core_record ${head_core_record} ${source_core_record})
SOURCE_GROUP(core/record FILES ${group_core_record})
FILE(GLOB head_result result/*.h)
FILE(GLOB source_result result/*.cpp)
SET(group_result ${head_result} ${source_result})
SOURCE_GROUP(result FILES ${group_result})
FILE(GLOB head_run run/*.h)
FILE(GLOB source_run run/*.cpp)
SET(group_run ${head_run} ${source_run})
SOURCE_GROUP(run FILES ${group_run})
FILE(GLOB head_utility utility/*.h)
FILE(GLOB source_utility utility/*.cpp)
SET(group_utility ${head_utility} ${source_utility})
SOURCE_GROUP(utility FILES ${group_utility})
FILE(GLOB head_utility_heap utility/heap/*.h)
FILE(GLOB source_utility_heap utility/heap/*.cpp)
SET(group_utility_heap ${head_utility_heap} ${source_utility_heap})
SOURCE_GROUP(utility/heap FILES ${group_utility_heap})
FILE(GLOB head_utility_linear_algebra utility/linear_algebra/*.h)
FILE(GLOB source_utility_linear_algebra utility/linear_algebra/*.cpp)
SET(group_utility_linear_algebra ${head_utility_linear_algebra} ${source_utility_linear_algebra})
SOURCE_GROUP(utility/linear_algebra FILES ${group_utility_linear_algebra})
FILE(GLOB head_utility_nondominated_sorting utility/nondominated_sorting/*.h)
FILE(GLOB source_utility_nondominated_sorting utility/nondominated_sorting/*.cpp)
SET(group_utility_nondominated_sorting ${head_utility_nondominated_sorting} ${source_utility_nondominated_sorting})
SOURCE_GROUP(utility/nondominated_sorting FILES ${group_utility_nondominated_sorting})
FILE(GLOB head_utility_random utility/random/*.h)
FILE(GLOB source_utility_random utility/random/*.cpp)
SET(group_utility_random ${head_utility_random} ${source_utility_random})
SOURCE_GROUP(utility/random FILES ${group_utility_random})
FILE(GLOB head_utility_typevar utility/typevar/*.h)
FILE(GLOB source_utility_typevar utility/typevar/*.cpp)
SET(group_utility_typevar ${head_utility_typevar} ${source_utility_typevar})
SOURCE_GROUP(utility/typevar FILES ${group_utility_typevar})

# ADD_EXECUTABLE

ADD_EXECUTABLE(OFEC_sc2
${group_core}
${group_core_algorithm}
${group_core_problem}
${group_core_problem_continuous}
${group_core_record}
${group_result}
${group_run}
${group_utility}
${group_utility_heap}
${group_utility_linear_algebra}
${group_utility_nondominated_sorting}
${group_utility_random}
${group_utility_typevar}
)