#include "read_elf.h"

#include <fstream>
#include <iostream>

ElfReader::ElfReader() {}

ElfReader::ElfReader(std::string filepath) {
  if (!std::filesystem::exists(filepath)) {
    throw(ElfReaderException("File does not exist"));
  }

  std::ifstream elf_file(filepath, std::ios::binary | std::ios::ate);
  std::streamsize size = elf_file.tellg();
  elf_file.seekg(0, std::ios::beg);

  if (!elf_file.read((char*)&header, sizeof(Elf32_Ehdr))) {
    elf_file.close();
    throw(ElfReaderException("Could not read file"));
  }

  // Check Magic Bytes
  for (int x = 0; x < 4; x++) {
    if (header.e_ident[x] != magic[x]) {
      elf_file.close();
      throw(ElfReaderException("Invalid magic bytes"));
    }
  }

  // Get Sections headers
  for (Elf32_Half section = 0; section < header.e_shnum; section++) {
    elf_file.seekg(header.e_shoff + (header.e_shentsize * section),
                   std::ios::beg);
    elf_file.read((char*)&section_header, sizeof(Elf32_Shdr));
    sections.push_back(section_header);
  }

  // Add to section map
  for (auto sec : sections) {
    elf_file.seekg(sections.at(header.e_shstrndx).sh_offset + sec.sh_name,
                   std::ios::beg);
    std::string name = "";
    char ch;
    while (true) {
      ch = elf_file.get();
      if (ch == '\0') {
        break;
      }
      name += ch;
    }
    m_section_map.emplace(name, sec);
  }

  // Get Program Headers
  for (Elf32_Half section = 0; section < header.e_phnum; section++) {
    elf_file.seekg(header.e_phoff + (header.e_phentsize * section),
                   std::ios::beg);
    elf_file.read((char*)&program_header, sizeof(Elf32_Phdr));
    loadable_headers.push_back(program_header);
  }
}

ElfReader::~ElfReader() {}

std::optional<section_map> ElfReader::GetSections() {
  if (m_section_map.size() > 0) {
    return m_section_map;
  }
  return {};
}

std::optional<std::vector<Elf32_Phdr>> ElfReader::GetLoadableSegments() {
  if (loadable_headers.size() > 0) {
    return loadable_headers;
  }
  return {};
}
