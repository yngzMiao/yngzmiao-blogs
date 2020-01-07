# get git hash
macro(get_git_hash _git_hash)   # 宏的开始
    find_package(Git QUIET)     # 查找Git，QUIET静默方式不报错
    if(GIT_FOUND)
      execute_process(          # 执行一个子进程
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h # 命令
        OUTPUT_VARIABLE ${_git_hash}        # 输出字符串存入变量
        OUTPUT_STRIP_TRAILING_WHITESPACE    # 删除字符串尾的换行符
        ERROR_QUIET                         # 对执行错误静默
        WORKING_DIRECTORY                   # 执行路径
          ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endmacro()                      # 宏的结束

# get git branch
macro(get_git_branch _git_branch)   # 宏的开始
    find_package(Git QUIET)     # 查找Git，QUIET静默方式不报错
    if(GIT_FOUND)
      execute_process(          # 执行一个子进程
        COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
        OUTPUT_VARIABLE ${_git_branch}        # 输出字符串存入变量
        OUTPUT_STRIP_TRAILING_WHITESPACE    # 删除字符串尾的换行符
        ERROR_QUIET                         # 对执行错误静默
        WORKING_DIRECTORY                   # 执行路径
          ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endmacro()                      # 宏的结束
