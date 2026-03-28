# Visual Studio Project Changes for wxWidgets 3.3.1

## Summary

Updated Visual Studio 2022 project files to use wxWidgets 3.3.1 instead of 3.0.3.

## Files Modified

1. `SSW.vcxproj`
2. `BattleSim\BattleSim.vcxproj`

## Changes Made

### Path Updates

**Include Directories:**
```
OLD: C:\Users\steph26\repos\wxWidgets-3.0.3\include\msvc
     C:\Users\steph26\repos\wxWidgets-3.0.3\include

NEW: C:\Users\steph26\repos\wxWidgets-3.3.1\include\msvc
     C:\Users\steph26\repos\wxWidgets-3.3.1\include
```

**Library Directories (Win32):**
```
OLD: C:\Users\steph26\repos\wxWidgets-3.0.3\lib\vc_lib
NEW: C:\Users\steph26\repos\wxWidgets-3.3.1\lib\vc_lib
```

**Library Directories (x64):**
```
OLD: C:\Users\steph26\repos\wxWidgets-3.0.3\lib\vc_x64_lib
NEW: C:\Users\steph26\repos\wxWidgets-3.3.1\lib\vc_x64_lib
```

### Library Name Updates

All wxWidgets library references changed from version 30 to 33:

**Debug Libraries:**
- `wxmsw30ud_core.lib` → `wxmsw33ud_core.lib`
- `wxbase30ud.lib` → `wxbase33ud.lib`
- `wxmsw30ud_adv.lib` → `wxmsw33ud_adv.lib`

**Release Libraries:**
- `wxmsw30u_core.lib` → `wxmsw33u_core.lib`
- `wxbase30u.lib` → `wxbase33u.lib`
- `wxmsw30u_adv.lib` → `wxmsw33u_adv.lib`

## Configurations Updated

Both projects updated for all configurations:
- Debug | Win32
- Debug | x64
- Release | Win32
- Release | x64

## Building

### Prerequisites

1. **Build wxWidgets 3.3.1 libraries:**
   ```
   Location: C:\Users\steph26\repos\wxWidgets-3.3.1\build\msw
   Open: wx_vc17.sln (for Visual Studio 2022)
   Build: All configurations you need (Debug/Release, Win32/x64)
   ```

2. **Verify library output:**
   - Win32 libraries: `C:\Users\steph26\repos\wxWidgets-3.3.1\lib\vc_lib\`
   - x64 libraries: `C:\Users\steph26\repos\wxWidgets-3.3.1\lib\vc_x64_lib\`

### Build SSW

1. Open `C:\Users\steph26\repos\SSW\SSW.sln`
2. Select configuration (e.g., Debug | x64)
3. Build → Build Solution (F7)

## Troubleshooting

### "Cannot open include file 'wx/xxx.h'"
- Ensure wxWidgets 3.3.1 include directories exist
- Check paths in project properties match the actual directory structure

### "Cannot open file 'wxmsw33xx.lib'"
- Ensure wxWidgets 3.3.1 has been built for the target configuration
- Check the library directory contains the expected .lib files
- Verify platform (Win32 vs x64) matches the selected configuration

### Link errors about undefined symbols
- Ensure you've built wxWidgets with the same runtime library setting (Multi-Threaded Debug)
- Verify all wxWidgets libraries are present in the lib directory

## Testing After Build

1. **Run Debug build first** to catch any runtime issues
2. **Test key functionality:**
   - Application startup (splash screen)
   - Window creation
   - Menu interactions
   - File operations

## Reverting Changes

If you need to revert to wxWidgets 3.0.3:
1. Use version control to restore the original .vcxproj files
2. OR manually change all paths back:
   - `wxWidgets-3.3.1` → `wxWidgets-3.0.3`
   - `wxmsw33` → `wxmsw30`
   - `wxbase33` → `wxbase30`
