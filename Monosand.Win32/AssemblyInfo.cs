﻿using System.Diagnostics.CodeAnalysis;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;

[assembly: CLSCompliant(true)]
#if !NETSTANDARD2_1
[assembly: SupportedOSPlatform("windows")]
#endif

[assembly: DefaultDllImportSearchPaths(DllImportSearchPath.SafeDirectories)]
[assembly: SuppressMessage("Usage", "CA2216")]

// for some uncomplete api
[assembly: InternalsVisibleTo("Test.Win32")]