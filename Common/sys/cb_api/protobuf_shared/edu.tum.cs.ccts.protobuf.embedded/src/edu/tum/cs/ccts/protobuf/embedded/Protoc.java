/*--------------------------------------------------------------------------+
|                                                                          |
| Copyright 2008-2012 Technische Universitaet Muenchen                     |
|                                                                          |
| Licensed under the Apache License, Version 2.0 (the "License");          |
| you may not use this file except in compliance with the License.         |
| You may obtain a copy of the License at                                  |
|                                                                          |
|    http://www.apache.org/licenses/LICENSE-2.0                            |
|                                                                          |
| Unless required by applicable law or agreed to in writing, software      |
| distributed under the License is distributed on an "AS IS" BASIS,        |
| WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. |
| See the License for the specific language governing permissions and      |
| limitations under the License.                                           |
+--------------------------------------------------------------------------*/

package edu.tum.cs.ccts.protobuf.embedded;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.HashMap;

import org.antlr.runtime.ANTLRInputStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.RecognitionException;
import org.antlr.runtime.tree.CommonTree;
import org.antlr.runtime.tree.CommonTreeNodeStream;
import org.antlr.stringtemplate.StringTemplateGroup;
import org.antlr.stringtemplate.language.DefaultTemplateLexer;

/**
 * A protocol buffers command-line compiler that generates c-files and c-headers
 * that can be used in resource constrained embedded systems.
 * <p>
 * Use the following command-line arguments to test:
 * <code> --debug -I=test --c_out=test Test.proto</code>
 * 
 * @author wolfgang.schwitzer
 * @author nvpopa
 */
public class Protoc {

	public static final String VERSION = "0.2 (2012-01-31)";

	private static boolean debug = false;

	public static void main(String[] args) throws Exception {
		// Display help screen.
		if (args.length == 0 || args[0].compareToIgnoreCase("-h") == 0) {
			System.out
					.println("Protoc Embedded C Compiler for Protocol Buffers");
			System.out.println("Version " + VERSION);
			System.out
					.println("Copyright (c) 2009-2011, Technische Universitaet Muenchen, http://www4.in.tum.de/");
			System.out
					.println("Use: Protoc [-I=<source-directory>][--c_out=<output-directory>] <proto-file>");
			System.out.println();
			System.exit(0);
		}
		// Parse command-line arguments.
		File sourceDirectory = null;
		File outputDirectory = null;
		for (String arg : args) {
			if (arg.startsWith("-I=")) {
				sourceDirectory = new File(arg.substring(3));
				if (!sourceDirectory.exists() || !sourceDirectory.isDirectory()) {
					System.out.println("Source directory '" + sourceDirectory
							+ "' does not exist or is not a directory");
					System.exit(1);
				}
			}
			if (arg.startsWith("--c_out=")) {
				outputDirectory = new File(arg.substring(8));
				if (!outputDirectory.exists() || !outputDirectory.isDirectory()) {
					System.out.println("Output directory '" + outputDirectory
							+ "' does not exist or is not a directory");
					System.exit(1);
				}
			}
			if (arg.equals("--debug")) {
				debug = true;
			}
		}
		if (sourceDirectory == null) {
			sourceDirectory = new File(System.getProperty("user.dir"));
		}
		if (outputDirectory == null) {
			outputDirectory = new File(System.getProperty("user.dir"));
		}
		File protoFile = new File(sourceDirectory, args[args.length - 1]);
		if (!protoFile.exists() || !protoFile.isFile()) {
			System.out.println("Proto file '" + protoFile
					+ "' does not exist or is not a file");
			System.exit(1);
		}
		// Parse input and build AST.
		final ANTLRInputStream input = new ANTLRInputStream(
				new FileInputStream(protoFile));
		final ProtoLexer lexer = new ProtoLexer(input);
		final CommonTokenStream tokens = new CommonTokenStream(lexer);
		final ProtoParser parser = new ProtoParser(tokens);
		final ProtoParser.proto_return proto = parser.proto();
		final CommonTree tree = proto.tree;
		if (debug) {
			TreePrintUtils.printTree(tree);
		}
		// Walk AST to check constraints.
		final CommonTreeNodeStream nodes = new CommonTreeNodeStream(tree);
		nodes.setTokenStream(tokens);
		final ConstraintChecker checker = new ConstraintChecker(nodes);
		checker.proto();
		if (checker.constraintErrors > 0) {
			System.err.println("ERROR(s): " + checker.constraintErrors
					+ " constraint violation(s), no files generated");
			System.exit(-1);
		}
		String name = protoFile.getName();
		if (name.contains(".")) {
			name = name.substring(0, name.lastIndexOf("."));
		}
		generate("embedded-h-file.stg", new File(outputDirectory, name + ".h"),
				tokens, tree, name, checker.topologicalOrder);
		generate("embedded-c-file.stg", new File(outputDirectory, name + ".c"),
				tokens, tree, name, checker.topologicalOrder);
	}

	private static void generate(String templateFilename, File outFile,
			CommonTokenStream tokens, CommonTree tree, String protoFilename,
			HashMap<String, Integer> topologicalOrder)
			throws FileNotFoundException, IOException, RecognitionException {
		// Load string template group file.
		final InputStream in = Protoc.class
				.getResourceAsStream(templateFilename);
		final Reader reader = new InputStreamReader(in);
		final StringTemplateGroup templates = new StringTemplateGroup(reader,
				DefaultTemplateLexer.class);
		// Walk AST to generate templates.
		final CommonTreeNodeStream nodes = new CommonTreeNodeStream(tree);
		nodes.setTokenStream(tokens);
		final EmbeddedCGen walker = new EmbeddedCGen(nodes);
		walker.setTemplateLib(templates);
		EmbeddedCGen.proto_return proto = walker.proto(protoFilename,
				topologicalOrder);
		// Emit File.
		final String result = proto.getTemplate().toString();
		if (debug) {
			System.out.println(result);
		}
		new FileOutputStream(outFile).write(result.getBytes());
	}
}
