// author: https://gist.github.com/mbant/67875e0464cd9d1402413532e3244261

#include <array>
#include <queue>
#include <string>
#include <utility>
#include <vector>

constexpr std::array<char, 26> docs{'a','b','c','d','e','f','g',
                                    'h','i','j','k','l','m','n',
                                    'o','p','q','r','s','t',
                                    'u','v','w','x','y','z'};

std::vector<std::vector<double>> log_p_language_model;
double lm(char cur, char next) {
  return log_p_language_model[cur - 'a'][next - 'a'];
}

// we want to decode a string of length 'length',
// starting for a given character
// and computing the next character by interfacing with our very sophisticated Language Model
// lm(current,next) that will return us the probability that the 'next' character follows the 'current'
// finally output the best string and its log_probability

// we can do it greedily (not optimal but easier to code)

class Rule {
 public:
  bool Match(int pos, int doc) { return true; }
  void Update(int pos, int doc) {}
};

struct Sequence {
  std::vector<int> selected;
  double score;
  Sequence() : score(0.0) {}
  Sequence(const std::vector<int>& selected_in, double score_in) : selected(selected_in), score(score_in) {}

  bool operator<(const Sequence& rhs) const { return score < rhs.score; }
};

using SeqVec = std::vector<Sequence>;
using SeqPQ = std::priority_queue<Sequence>;

Sequence greedy_decoding(char start, int length) {
  Sequence seq;
  auto& selected = seq.selected;
  selected.reserve(length);
  selected.push_back(start);

  char chosen = '\0';
  double max_score = 0.0;
  for (int i = 1; i < length; ++i) {
    for (const auto& ch : docs) {
      double tmp_score = lm(selected.back(), ch);
      if (tmp_score > max_score) {
        max_score = tmp_score;
        chosen = ch;
      }
    }
    selected.push_back(chosen);
    seq.score += max_score;
  }
  return seq;
}

void expand_next_seqs(const Sequence& best_seq, int expand_size, int beam_size,
                      SeqPQ& next_seqs, std::vector<Rule>& rules) {
  int pos = best_seq.selected.size();
  SeqPQ expand_seqs;

  for (const auto& doc : docs) {
    bool found = true;
    for (auto& rule : rules) {
      if (not rule.Match(pos, doc)) {
        found = false;
        break;
      }
    }
    if (not found) continue;

    auto expand_seq = best_seq.selected;
    expand_seq.push_back(doc);
    double cur_score = best_seq.score + lm(best_seq.selected.back(), doc);
    expand_seqs.emplace(expand_seq, cur_score);

    if (expand_seqs.size() == expand_size) break;
  }


  for (int i = 0; i < beam_size and not expand_seqs.empty(); ++i, expand_seqs.pop()) {
    next_seqs.push(expand_seqs.top());
  }
}

void expand_all_next_seqs(const SeqVec& best_seqs, int expand_size, int beam_size,
                          SeqPQ& next_seqs, std::vector<Rule>& rules) {
  for (const auto& best_seq : best_seqs) {
    expand_next_seqs(best_seq, expand_size, beam_size, next_seqs, rules);
  }
}

void update_best_seqs(SeqPQ& next_seqs, SeqVec& best_seqs, int beam_size) {
  best_seqs.clear();
  best_seqs.reserve(beam_size);
  for (int i = 0; i < beam_size and not next_seqs.empty(); ++i, next_seqs.pop()) {
    best_seqs.push_back(next_seqs.top());
  }
}

//Sequence beam_decoding(char start, int length, int beam_size) {
//  SeqVec best_seqs(1);
//  best_seqs[0].str[0] = start;
//
//  for (int i = 1; i < length; ++i) {
//    SeqPQ next_seqs;
//    expand_all_next_seqs(best_seqs, next_seqs, beam_size);
//    update_best_seqs(next_seqs, best_seqs, beam_size);
//  }
//
//  double max_score = best_seqs[0].score;
//  int selected = 0;
//  for (int i = 1, sz = best_seqs.size(); i < sz; ++i) {
//    if (best_seqs[i].score > max_score) {
//      selected = i;
//      max_score = best_seqs[i].score;
//    }
//  }
//  return best_seqs[selected];
//}

int main() {
  return 0;
}
