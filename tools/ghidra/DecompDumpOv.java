// Ghidra headless script: overlay-aware decompile-to-C dump.
//
// Same job as DecompDump.java, but addresses are resolved inside a named memory
// block instead of the default address space. On a dsd-ghidra loaded ROM each
// overlay is its own Ghidra overlay space, and several overlays share the same
// numeric address -- the very ambiguity that shows up as `module:overlays(2,7)` in
// config/**/relocs.txt. DecompDump.java's bare toAddr() cannot express which one
// is meant, so it silently decompiles whatever the default space holds.
//
// Targets file, one per line, '#' comments and blank lines ignored:
//     <block> <0xADDR> [name]
// e.g.
//     ov102 0x0214b53c func_ov102_0214b53c
//     ov006 0x020dbe9c func_ov006_020dbe9c
// Use block "-" to resolve in the default space (ARM9 main).
//
// Output: <outDir>/<block>_<0xADDR>.c, plus a TSV summary on stdout.
//
//   analyzeHeadless <proj> <name> -process <prog> -noanalysis \
//       -scriptPath tools/ghidra -postScript DecompDumpOv.java <targets> <outDir>
//@category dsd
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.PrintWriter;

public class DecompDumpOv extends GhidraScript {

    private Address resolve(String blockName, long addr) {
        if (blockName.equals("-")) {
            return toAddr(addr);
        }
        Memory memory = currentProgram.getMemory();
        MemoryBlock block = memory.getBlock(blockName);
        if (block == null) {
            // fall back to a case-insensitive scan; dsd names blocks per overlay
            for (MemoryBlock b : memory.getBlocks()) {
                if (b.getName().equalsIgnoreCase(blockName)) {
                    block = b;
                    break;
                }
            }
        }
        if (block == null) {
            return null;
        }
        Address a = block.getStart().getAddressSpace().getAddress(addr);
        return block.contains(a) ? a : null;
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length < 2) {
            println("usage: DecompDumpOv <targetsPath> <outDir>");
            return;
        }
        File outDir = new File(args[1]);
        outDir.mkdirs();

        DecompInterface di = new DecompInterface();
        di.openProgram(currentProgram);

        int ok = 0, fail = 0;
        BufferedReader br = new BufferedReader(new FileReader(args[0]));
        String line;
        while ((line = br.readLine()) != null) {
            line = line.trim();
            if (line.isEmpty() || line.startsWith("#")) {
                continue;
            }
            String[] tok = line.split("\\s+");
            if (tok.length < 2) {
                println("SKIP\t" + line + "\tmalformed");
                fail++;
                continue;
            }
            String blockName = tok[0];
            long raw;
            try {
                raw = Long.decode(tok[1]);
            } catch (NumberFormatException e) {
                println("SKIP\t" + line + "\tbad address");
                fail++;
                continue;
            }

            Address addr = resolve(blockName, raw);
            if (addr == null) {
                println(String.format("FAIL\t%s\t%s\tno such block, or address outside it",
                    blockName, tok[1]));
                fail++;
                continue;
            }

            Function f = getFunctionAt(addr);
            if (f == null) {
                f = createFunction(addr, null);
            }
            if (f == null) {
                println(String.format("FAIL\t%s\t%s\tno function and could not create one",
                    blockName, tok[1]));
                fail++;
                continue;
            }

            DecompileResults res = di.decompileFunction(f, 120, monitor);
            if (res == null || !res.decompileCompleted()
                    || res.getDecompiledFunction() == null) {
                println(String.format("FAIL\t%s\t%s\t%s\tdecompile failed: %s",
                    blockName, tok[1], f.getName(),
                    res == null ? "null" : res.getErrorMessage()));
                fail++;
                continue;
            }

            File out = new File(outDir, blockName + "_" + tok[1] + ".c");
            PrintWriter pw = new PrintWriter(out);
            pw.print(res.getDecompiledFunction().getC());
            pw.close();
            println(String.format("OK\t%s\t%s\t%s\tbody=0x%x\t%s",
                blockName, tok[1], f.getName(), f.getBody().getNumAddresses(),
                out.getName()));
            ok++;
        }
        br.close();
        println("DecompDumpOv done: ok=" + ok + " fail=" + fail);
    }
}
