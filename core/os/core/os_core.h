#pragma once

// Replace your filesystem code with this:
struct Path_Info {
    std::string dirpath;
    std::string name;
};

fn get_absolute_path(std::string_view filename) -> std::string;
fn get_parent_path(std::string_view path) -> std::string;
fn get_stem(std::string_view path) -> std::string;
fn get_filename(std::string_view path) -> std::string;
fn get_extension(std::string_view path) -> std::string;
fn get_path_info(std::string_view filename) -> Path_Info;

fn os_read_entire_file(std::string_view filename) -> std::string;
fn os_write_entire_file(std::string_view filename, std::string_view content) -> bool;
fn os_trim(std::string text) -> std::string;

// @Note: This is relative to the exe path. Ex: "\\..\\..\\assets" would set the wdir two folders up the exe, inside the assets dir.
fn os_set_working_dir(const std::string& path) -> void;
fn os_get_time() -> f64;