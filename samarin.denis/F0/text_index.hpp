#ifndef TEXT_INDEX_HPP
#define TEXT_INDEX_HPP

#include <cstddef>
#include <string>
#include <list.hpp>
#include "cuckoo_table.hpp"
#include "hashers.hpp"
#include "text.hpp"

namespace samarin {

  using PostingList = List< position_t >;
  using PostingsTable = CuckooTable< std::string, PostingList, StringHash >;

  class TextIndex {
  public:
    void build(const Text& text);
    Text restore() const;

    bool has(const std::string& word) const;
    const PostingList& positions(const std::string& word) const;
    std::size_t lineCount() const;
    const PostingsTable& postings() const;

  private:
    PostingsTable postings_;
    List< std::size_t > lineLengths_;
  };

}

#endif
