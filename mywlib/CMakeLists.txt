cmake_minimum_required(VERSION 3.10)

project(OS-Course-Design)

aux_source_directory(. dir_srcs)
add_library(mywlib ${dir_srcs})