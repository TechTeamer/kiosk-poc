cmd_Release/pr22.node := ln -f "Release/obj.target/pr22.node" "Release/pr22.node" 2>/dev/null || (rm -rf "Release/pr22.node" && cp -af "Release/obj.target/pr22.node" "Release/pr22.node")
