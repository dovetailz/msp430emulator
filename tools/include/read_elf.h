#ifndef read_elf_h
#define read_elf_h

#include <elf.h>

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

typedef std::map<std::string, Elf32_Shdr> section_map;

class ElfReader {
 public:
  ElfReader();
  ElfReader(std::string filepath);
  ~ElfReader();

  std::optional<section_map> GetSections();
  std::optional<std::vector<Elf32_Phdr>> GetLoadableSegments();

  Elf32_Ehdr header;
  Elf32_Shdr section_header;
  std::optional<Elf32_Shdr> symbol_section;
  Elf32_Sym symbol;
  Elf32_Phdr program_header;

 private:
  std::vector<Elf32_Shdr> sections;
  std::vector<Elf32_Sym> symbols;
  std::vector<Elf32_Phdr> loadable_headers;
  section_map m_section_map;
  uint8_t magic[4]{0x7f, 0x45, 0x4c, 0x46};
};

class ElfReaderException : public std::exception {
  std::string _msg;

 public:
  ElfReaderException(const std::string& msg) : _msg(msg) {}

  virtual const char* what() const noexcept override { return _msg.c_str(); }
};

#endif