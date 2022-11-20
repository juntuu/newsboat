#ifndef NEWSBOAT_FILE_SYSTEM_H_
#define NEWSBOAT_FILE_SYSTEM_H_

#include <string>
#include <sys/stat.h>

#include "utf8string.h"

#include "3rd-party/optional.hpp"

namespace newsboat {

namespace file_system {

enum class FileType {
	Unknown,
	RegularFile,
	Directory,
	BlockDevice,
	CharDevice,
	Fifo,
	Symlink,
	Socket,
};

FileType mode_to_filetype(mode_t mode);

/// Convert the filetype into an ASCII character that `ls` and others use to
/// represent it.
///
/// \note `Unknown` filetype is represented by a question mark.
char filetype_to_char(FileType type);

/// Get a suffix that `ls --classify` adds to names with given `mode`.
nonstd::optional<char> mode_suffix(mode_t mode);

/// An entry in a listing, e.g. a file, a directory or suchlike.
struct FileSystemEntry {
	FileType filetype;
	Utf8String name;
};

/// The name of the user with a given UID, padded on the right to the width of
/// 8 characters. If the name is unknown, returns 8 question marks.
Utf8String get_user_padded(uid_t uid);

/// The name of the group with a given GID, padded on the right to the width of
/// 8 characters. If the name is unknown, returns 8 question marks.
Utf8String get_group_padded(gid_t gid);

/// Convert permissions into an rwxrwxrwx-style string.
Utf8String permissions_string(mode_t mode);

} // namespace file_system

} // namespace newsboat

#endif //NEWSBOAT_FILE_SYSTEM_H_
