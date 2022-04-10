require "android-lib"

workspace "libzip"
    configurations { "Debug", "Release" }
    location ("projects/" .. os.target())

project "libzip"
    kind "StaticLib"
    language "C"
    
    location ("projects/" .. os.target())
    targetname "zip"

    files {
        "src/compat.h",
        "src/config.h",
        "src/zip.h",
        "src/zipconf.h",
        "src/zipint.h",
        "src/zipwin32.h",
        "src/zip_add.c",
        "src/zip_add_dir.c",
        "src/zip_add_entry.c",
        "src/zip_algorithm_deflate.c",
        "src/zip_algorithm_xz.c",
        "src/zip_buffer.c",
        "src/zip_close.c",
        "src/zip_delete.c",
        "src/zip_dirent.c",
        "src/zip_dir_add.c",
        "src/zip_discard.c",
        "src/zip_entry.c",
        "src/zip_error.c",
        "src/zip_error_clear.c",
        "src/zip_error_get.c",
        "src/zip_error_get_sys_type.c",
        "src/zip_error_strerror.c",
        "src/zip_error_to_str.c",
        "src/zip_err_str.c",
        "src/zip_extra_field.c",
        "src/zip_extra_field_api.c",
        "src/zip_fclose.c",
        "src/zip_fdopen.c",
        "src/zip_filerange_crc.c",
        "src/zip_file_add.c",
        "src/zip_file_error_clear.c",
        "src/zip_file_error_get.c",
        "src/zip_file_get_comment.c",
        "src/zip_file_get_external_attributes.c",
        "src/zip_file_get_offset.c",
        "src/zip_file_rename.c",
        "src/zip_file_replace.c",
        "src/zip_file_set_comment.c",
        "src/zip_file_set_encryption.c",
        "src/zip_file_set_external_attributes.c",
        "src/zip_file_set_mtime.c",
        "src/zip_file_strerror.c",
        "src/zip_fopen.c",
        "src/zip_fopen_encrypted.c",
        "src/zip_fopen_index.c",
        "src/zip_fopen_index_encrypted.c",
        "src/zip_fread.c",
        "src/zip_fseek.c",
        "src/zip_ftell.c",
        "src/zip_get_archive_comment.c",
        "src/zip_get_archive_flag.c",
        "src/zip_get_encryption_implementation.c",
        "src/zip_get_file_comment.c",
        "src/zip_get_name.c",
        "src/zip_get_num_entries.c",
        "src/zip_get_num_files.c",
        "src/zip_hash.c",
        "src/zip_io_util.c",
        "src/zip_libzip_version.c",
        "src/zip_memdup.c",
        "src/zip_name_locate.c",
        "src/zip_new.c",
        "src/zip_open.c",
        "src/zip_progress.c",
        "src/zip_rename.c",
        "src/zip_replace.c",
        "src/zip_set_archive_comment.c",
        "src/zip_set_archive_flag.c",
        "src/zip_set_default_password.c",
        "src/zip_set_file_comment.c",
        "src/zip_set_file_compression.c",
        "src/zip_set_name.c",
        "src/zip_source_accept_empty.c",
        "src/zip_source_begin_write.c",
        "src/zip_source_begin_write_cloning.c",
        "src/zip_source_buffer.c",
        "src/zip_source_call.c",
        "src/zip_source_close.c",
        "src/zip_source_commit_write.c",
        "src/zip_source_compress.c",
        "src/zip_source_crc.c",
        "src/zip_source_error.c",
        "src/zip_source_filep.c",
        "src/zip_source_free.c",
        "src/zip_source_function.c",
        "src/zip_source_get_compression_flags.c",
        "src/zip_source_is_deleted.c",
        "src/zip_source_layered.c",
        "src/zip_source_open.c",
        "src/zip_source_pkware.c",
        "src/zip_source_read.c",
        "src/zip_source_remove.c",
        "src/zip_source_rollback_write.c",
        "src/zip_source_seek.c",
        "src/zip_source_seek_write.c",
        "src/zip_source_stat.c",
        "src/zip_source_supports.c",
        "src/zip_source_tell.c",
        "src/zip_source_tell_write.c",
        "src/zip_source_win32handle.c",
        "src/zip_source_win32utf8.c",
        "src/zip_source_win32w.c",
        "src/zip_source_window.c",
        "src/zip_source_write.c",
        "src/zip_source_zip.c",
        "src/zip_source_zip_new.c",
        "src/zip_stat.c",
        "src/zip_stat_index.c",
        "src/zip_stat_init.c",
        "src/zip_strerror.c",
        "src/zip_string.c",
        "src/zip_unchange.c",
        "src/zip_unchange_all.c",
        "src/zip_unchange_archive.c",
        "src/zip_unchange_data.c",
        "src/zip_utf-8.c"
    }

    sysincludedirs {
        "src",
        "../zlib/source",
        "../xz-utils/src/liblzma/api"
    }

    androidabis {
		"armeabi-v7a", "arm64-v8a"
    }

    androidminsdkversion "android-16"
    
    filter { "action:vs*" }
        objdir ("!obj/$(PlatformName)/$(Configuration)")
        targetdir ("lib/$(PlatformName)/$(Configuration)")
        characterset "MBCS"

    filter { "action:xcode*" }
        objdir ("!obj/$(PLATFORM_NAME)/$(CONFIGURATION)")
        targetdir ("lib/$(PLATFORM_NAME)/$(CONFIGURATION)")
    
    filter { "action:android*" }
        objdir ("obj/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")
        targetdir ("lib/android/$(TARGET_ARCH_ABI)/$(APP_OPTIM)")

    filter {}
        defines {
            "HAVE_CONFIG_H",
            "_CRT_SECURE_NO_DEPRECATE",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"
        inlining "Auto"
        floatingpoint "Fast"
        vectorextensions "SSE2"

    filter { "action:xcode*" }
        xcodebuildsettings {           
            ["CLANG_CXX_LANGUAGE_STANDARD"] = "gnu++20";
            ["GCC_C_LANGUAGE_STANDARD"] = "gnu11";
            ["CLANG_CXX_LIBRARY"] = "libc++";
        }
    
    filter { "action:vs*" }
        cppdialect "C++20"
        cdialect "C11"