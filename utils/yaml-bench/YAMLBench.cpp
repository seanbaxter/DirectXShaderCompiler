//===- YAMLBench - Benchmark the YAMLParser implementation ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This program executes the YAMLParser on differently sized YAML texts and
// outputs the run time.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ADT/SmallString.h"
#include "llvm37/Support/Casting.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/SourceMgr.h"
#include "llvm37/Support/Timer.h"
#include "llvm37/Support/Process.h"
#include "llvm37/Support/YAMLParser.h"
#include "llvm37/Support/raw_ostream.h"
#include <system_error>

using namespace llvm37;

static cl::opt<bool>
  DumpTokens( "tokens"
            , cl::desc("Print the tokenization of the file.")
            , cl::init(false)
            );

static cl::opt<bool>
  DumpCanonical( "canonical"
               , cl::desc("Print the canonical YAML for this file.")
               , cl::init(false)
               );

static cl::opt<std::string>
 Input(cl::Positional, cl::desc("<input>"));

static cl::opt<bool>
  Verify( "verify"
        , cl::desc(
            "Run a quick verification useful for regression testing")
        , cl::init(false)
        );

static cl::opt<unsigned>
  MemoryLimitMB("memory-limit", cl::desc(
                  "Do not use more megabytes of memory"),
                cl::init(1000));

cl::opt<cl::boolOrDefault>
    UseColor("use-color", cl::desc("Emit colored output (default=autodetect)"),
             cl::init(cl::BOU_UNSET));

struct indent {
  unsigned distance;
  indent(unsigned d) : distance(d) {}
};

static raw_ostream &operator <<(raw_ostream &os, const indent &in) {
  for (unsigned i = 0; i < in.distance; ++i)
    os << "  ";
  return os;
}

/// \brief Pretty print a tag by replacing tag:yaml.org,2002: with !!.
static std::string prettyTag(yaml::Node *N) {
  std::string Tag = N->getVerbatimTag();
  if (StringRef(Tag).startswith("tag:yaml.org,2002:")) {
    std::string Ret = "!!";
    Ret += StringRef(Tag).substr(18);
    return Ret;
  }
  std::string Ret = "!<";
  Ret += Tag;
  Ret += ">";
  return Ret;
}

static void dumpNode( yaml::Node *n
                    , unsigned Indent = 0
                    , bool SuppressFirstIndent = false) {
  if (!n)
    return;
  if (!SuppressFirstIndent)
    outs() << indent(Indent);
  StringRef Anchor = n->getAnchor();
  if (!Anchor.empty())
    outs() << "&" << Anchor << " ";
  if (yaml::ScalarNode *sn = dyn_cast<yaml::ScalarNode>(n)) {
    SmallString<32> Storage;
    StringRef Val = sn->getValue(Storage);
    outs() << prettyTag(n) << " \"" << yaml::escape(Val) << "\"";
  } else if (yaml::BlockScalarNode *BN = dyn_cast<yaml::BlockScalarNode>(n)) {
    outs() << prettyTag(n) << " \"" << yaml::escape(BN->getValue()) << "\"";
  } else if (yaml::SequenceNode *sn = dyn_cast<yaml::SequenceNode>(n)) {
    outs() << prettyTag(n) << " [\n";
    ++Indent;
    for (yaml::SequenceNode::iterator i = sn->begin(), e = sn->end();
                                      i != e; ++i) {
      dumpNode(i, Indent);
      outs() << ",\n";
    }
    --Indent;
    outs() << indent(Indent) << "]";
  } else if (yaml::MappingNode *mn = dyn_cast<yaml::MappingNode>(n)) {
    outs() << prettyTag(n) << " {\n";
    ++Indent;
    for (yaml::MappingNode::iterator i = mn->begin(), e = mn->end();
                                     i != e; ++i) {
      outs() << indent(Indent) << "? ";
      dumpNode(i->getKey(), Indent, true);
      outs() << "\n";
      outs() << indent(Indent) << ": ";
      dumpNode(i->getValue(), Indent, true);
      outs() << ",\n";
    }
    --Indent;
    outs() << indent(Indent) << "}";
  } else if (yaml::AliasNode *an = dyn_cast<yaml::AliasNode>(n)){
    outs() << "*" << an->getName();
  } else if (isa<yaml::NullNode>(n)) {
    outs() << prettyTag(n) << " null";
  }
}

static void dumpStream(yaml::Stream &stream) {
  for (yaml::document_iterator di = stream.begin(), de = stream.end(); di != de;
       ++di) {
    outs() << "%YAML 1.2\n"
           << "---\n";
    yaml::Node *n = di->getRoot();
    if (n)
      dumpNode(n);
    else
      break;
    outs() << "\n...\n";
  }
}

static void benchmark( llvm37::TimerGroup &Group
                     , llvm37::StringRef Name
                     , llvm37::StringRef JSONText) {
  llvm37::Timer BaseLine((Name + ": Loop").str(), Group);
  BaseLine.startTimer();
  char C = 0;
  for (llvm37::StringRef::iterator I = JSONText.begin(),
                                 E = JSONText.end();
       I != E; ++I) { C += *I; }
  BaseLine.stopTimer();
  volatile char DontOptimizeOut = C; (void)DontOptimizeOut;

  llvm37::Timer Tokenizing((Name + ": Tokenizing").str(), Group);
  Tokenizing.startTimer();
  {
    yaml::scanTokens(JSONText);
  }
  Tokenizing.stopTimer();

  llvm37::Timer Parsing((Name + ": Parsing").str(), Group);
  Parsing.startTimer();
  {
    llvm37::SourceMgr SM;
    llvm37::yaml::Stream stream(JSONText, SM);
    stream.skip();
  }
  Parsing.stopTimer();
}

static std::string createJSONText(size_t MemoryMB, unsigned ValueSize) {
  std::string JSONText;
  llvm37::raw_string_ostream Stream(JSONText);
  Stream << "[\n";
  size_t MemoryBytes = MemoryMB * 1024 * 1024;
  while (JSONText.size() < MemoryBytes) {
    Stream << " {\n"
           << "  \"key1\": \"" << std::string(ValueSize, '*') << "\",\n"
           << "  \"key2\": \"" << std::string(ValueSize, '*') << "\",\n"
           << "  \"key3\": \"" << std::string(ValueSize, '*') << "\"\n"
           << " }";
    Stream.flush();
    if (JSONText.size() < MemoryBytes) Stream << ",";
    Stream << "\n";
  }
  Stream << "]\n";
  Stream.flush();
  return JSONText;
}

int main(int argc, char **argv) {
  llvm37::cl::ParseCommandLineOptions(argc, argv);
  bool ShowColors = UseColor == cl::BOU_UNSET
                        ? sys::Process::StandardOutHasColors()
                        : UseColor == cl::BOU_TRUE;
  if (Input.getNumOccurrences()) {
    ErrorOr<std::unique_ptr<MemoryBuffer>> BufOrErr =
        MemoryBuffer::getFileOrSTDIN(Input);
    if (!BufOrErr)
      return 1;
    MemoryBuffer &Buf = *BufOrErr.get();

    llvm37::SourceMgr sm;
    if (DumpTokens) {
      yaml::dumpTokens(Buf.getBuffer(), outs());
    }

    if (DumpCanonical) {
      yaml::Stream stream(Buf.getBuffer(), sm, ShowColors);
      dumpStream(stream);
      if (stream.failed())
        return 1;
    }
  }

  if (Verify) {
    llvm37::TimerGroup Group("YAML parser benchmark");
    benchmark(Group, "Fast", createJSONText(10, 500));
  } else if (!DumpCanonical && !DumpTokens) {
    llvm37::TimerGroup Group("YAML parser benchmark");
    benchmark(Group, "Small Values", createJSONText(MemoryLimitMB, 5));
    benchmark(Group, "Medium Values", createJSONText(MemoryLimitMB, 500));
    benchmark(Group, "Large Values", createJSONText(MemoryLimitMB, 50000));
  }

  return 0;
}
