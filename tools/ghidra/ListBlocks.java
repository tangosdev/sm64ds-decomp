// Ghidra headless script: print the memory map of a dsd-ghidra loaded DS ROM.
//
// The dsd-ghidra loader gives each overlay its own Ghidra overlay address space,
// so a bare `toAddr(0x0214b53c)` is ambiguous (or lands in main ARM9) exactly the
// way an overlay-ambiguous relocation is. This dumps block name -> space + range so
// the dump scripts can address an overlay explicitly.
//
//   analyzeHeadless <proj> <name> -process <prog> -noanalysis \
//       -scriptPath tools/ghidra -postScript ListBlocks.java
//@category dsd
import ghidra.app.script.GhidraScript;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;

public class ListBlocks extends GhidraScript {
    @Override
    public void run() throws Exception {
        Memory memory = currentProgram.getMemory();
        println("BLOCKS " + memory.getBlocks().length);
        for (MemoryBlock b : memory.getBlocks()) {
            println(String.format(
                "BLOCK\t%s\t%s\t%s\t%s\toverlay=%b\tinit=%b\t%s%s%s",
                b.getName(),
                b.getStart().getAddressSpace().getName(),
                b.getStart(),
                b.getEnd(),
                b.isOverlay(),
                b.isInitialized(),
                b.isRead() ? "r" : "-",
                b.isWrite() ? "w" : "-",
                b.isExecute() ? "x" : "-"
            ));
        }
        println("FUNCTIONS " + currentProgram.getFunctionManager().getFunctionCount());
    }
}
