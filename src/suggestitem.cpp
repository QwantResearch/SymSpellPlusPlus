#include "suggestitem.h"


namespace symspell {

    SuggestItem::SuggestItem(const symspell::SuggestItem & p)
    {
        this->count = p.count;
        this->distance = p.distance;
        this->term = p.term;
    }

    SuggestItem::SuggestItem(string term, int distance, long count)
    {
        this->term = term;
        this->distance = distance;
        this->count = count;
    }

    SuggestItem::~SuggestItem(){}
//     {
//         delete term;
//     }
  
    bool SuggestItem::CompareTo(SuggestItem const& other)
    {
        // order by distance ascending, then by frequency count descending
        if (this->distance == other.distance)
        {
            if (other.count == this->count)
                return false;
            else if (other.count > this->count)
                return true;
            return false;
        }

        if (other.distance > this->distance)
            return false;
        return true;
    }

    bool SuggestItem::operator == (const SuggestItem &ref) const
    {
//         return strcmp(this->term, ref.term) == 0;
        return this->term.compare(ref.term) == 0;
    }

    std::size_t SuggestItem::GetHashCode()
    {
        return hash_c_string{}(term);
    }

    SuggestItem SuggestItem::ShallowCopy()
    {
        SuggestItem item;
        item.count = this->count;
        item.distance = this->distance;
        item.term = this->term;

        return item;
    }
    
    std::string SuggestItem::ToString()
    {
        cout << this->term << "\t" << this->count << "\t"<< this->distance << endl;
    }
}