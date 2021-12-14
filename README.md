UnionFS
=======

This simple FUSE-based filesystem is just a wrapper that allows to "merge" few mountpoints into one. It is like software replacement for RAID. The effect of this is somehow the same as creating RAID0, but this works in terms of mountpoints rather than devices, so even few partitions of the same drive can be "merged" without any actual partition table changes. From other side, unlike RAID array creation, it does not require any data destruction for its initialization and disks (mountpoints) order doesn't matter. Again, comparing to RAID0, this has some kind of load balancing (but pretty specific, details below).

In general, this is POSIX-compliant FS with few exceptions due to implementation and FUSE specific itself. But real available FS features set mostly depends on underlying FS and may vary.

There are no any specific restrictions to underlying FS, even read-only mountpoints can be used, but it is recommended (especially in case of RW) to use the same FS for all underlying mountpoints.

Balancing
---------

FS has the concept of "shared folders", which only participate in load balancing. Unfortunately, such folders can't be created just by `mkdir` called somewhere in this filesystem mountpoint. Such folders must be created manually in each mountpoint separately. Folder becomes "shared" when few (but not strictly all) source mountpoints have folder with the same name at same level. Root is "shared" by its nature (each mountpoint is root). When new file or folder should be created in "shared" folder, FS picks one mountpoint (based on available space) from all available which shared folder belongs to and creates it here. In case of file/folder creation in non-shared folder, new item is created just in it (no mountpoint picking happens) as on any normal filesystem. For subsequent "create" calls in "shared" folders FS just iterate through available mountpoints choosing new one as target each time. But after some time (configurable) it "forgets" last used mountpoint and starts iteration from scratch. So in case rare "create" calls all new items will be created only in mountpoint which has more available space at that moment rather than others.

As expected, read-only mountpoints are excluded from possible targets list in case of load balancing. Any RW mountpoint also can be explicitly excluded just by setting an option in config.

Without "shared folders" (except root, which is "shared" by default) no any balancing will happen.

Configuration
-------------

Configuration file used by this service has simple INI-like syntax and must be placed to `/etc/unionfs.conf`. FS service can't work without this config containing at least 1 mountpoint. Example config (with description of all keys) below.

```conf
# please note, inline (aka at the end of line) comments are NOT supported
# any line started with # (sharp) or ; (semicolon) is considered as comment
# spaces in keys are supported, this feature was taken from Samba' config

# section containing global settings, section name is fixed
[global]
# timeout in seconds after which FS forgets last used mountpoint
# participating in load balancing, and picks new one based on space
disk cache timeout = 3600

# source mountpoint description, section name is arbitrary string
# config can contain any number of such sections, but at least one
# section names must be unique
[disk 1]
# path to mountpoint
mountpoint = /mnt/disk1

[disk 2]
mountpoint = /mnt/disk2
# option to exclude mountpoint from load balancing participation
# (commented in this example, due to ';' at the beginning)
; no shared writes = true

[disk 3]
mountpoint = /mnt/disk3
```

Running
-------

This FS service is normal FUSE-based application, so any options supported by corresponding FUSE version are allowed. Path to mountpoint (where to mount this FS) is also must be specified as command line argument (as for any other FUSE-based app). There are no app-specific command line options.

List of available command line options vary depending on FUSE version and can be retrieved though `-h`. Detailed option description can be found in FUSE documentation (short description is displayed in help message).

There is only one important note - `default_permissions` must be specified as one of mount options (regardless of FUSE version), implementation relies on it. Also it is recommended to pass `allow_other` and run service as root due to FUSE specific.

History
-------

As afterwords, short history of this strange thing :) I had 2 8TB HDDs with a lot of data on each of them. But having 2 mountpoints became too inconvenient starting from some time (when I decided to split some content). I didn't want to lose all I had (due to RAID array initialization) and also didn't have space to copy all my data, so I decided instead of creating RAID array to implement simple filesystem to achieve desired effect, just because it easy for me and even faster rather than copying large amount of data :) (first working version just to try the concept was written in ~2 hours!).
