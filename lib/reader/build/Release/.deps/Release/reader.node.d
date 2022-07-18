cmd_Release/reader.node := ln -f "Release/obj.target/reader.node" "Release/reader.node" 2>/dev/null || (rm -rf "Release/reader.node" && cp -af "Release/obj.target/reader.node" "Release/reader.node")
