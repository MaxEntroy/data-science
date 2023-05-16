// author: https://gist.github.com/mbant/67875e0464cd9d1402413532e3244261

#include <array>
#include <queue>
#include <string>
#include <utility>
#include <vector>

constexpr std::array<char, 26> kVocab{'a','b','c','d','e','f','g',
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

struct Sequence {
  std::string str;
  double score;
  Sequence() : score(0.0) {}
  Sequence(const std::string& s, double score_in) : str(s), score(score_in) {}

  bool operator<(const Sequence& rhs) const { return score < rhs.score; }
};

using SeqVector = std::vector<Sequence>;
using SeqPQ = std::priority_queue<Sequence>;

Sequence greedy_decoding(char start, int length) {
  Sequence seq;
  auto& s = seq.str;
  s.reserve(length);
  s.push_back(start);

  char chosen = '\0';
  double max_score = 0.0;
  for (int i = 1; i < length; ++i) {
    for (const auto& ch : kVocab) {
      double tmp_score = lm(s.back(), ch);
      if (tmp_score > max_score) {
        max_score = tmp_score;
        chosen = ch;
      }
    }
    s.push_back(chosen);
    seq.score += max_score;
  }
  return seq;
}

void expand_next_seqs(const Sequence& best_seq, SeqPQ& next_seqs, int branching_size) {
  SeqPQ next_possible_seqs;

  for (const auto& ch : kVocab) {
    std::string cur_str = best_seq.str + ch;
    double cur_score = best_seq.score + lm(best_seq.str.back(), ch);
    next_possible_seqs.emplace(cur_str, cur_score);
  }

  for (int i = 0; i < branching_size and not next_possible_seqs.empty(); ++i, next_possible_seqs.pop()) {
    next_seqs.push(next_possible_seqs.top());
  }
}

void expand_all_next_seqs(const SeqVector& best_seqs, SeqPQ& next_seqs, int branching_size) {
  for (const auto& best_seq : best_seqs) {
    expand_next_seqs(best_seq, next_seqs, branching_size);
  }
}

void update_best_seqs(SeqPQ& next_seqs, SeqVector& best_seqs, int beam_size) {
  best_seqs.clear();
  best_seqs.reserve(beam_size);
  for (int i = 0; i < beam_size and not next_seqs.empty(); ++i, next_seqs.pop()) {
    best_seqs.push_back(next_seqs.top());
  }
}

Sequence beam_decoding(char start, int length, int beam_size) {
  SeqVector best_seqs(1);
  best_seqs[0].str[0] = start;

  for (int i = 1; i < length; ++i) {
    SeqPQ next_seqs;
    expand_all_next_seqs(best_seqs, next_seqs, beam_size);
    update_best_seqs(next_seqs, best_seqs, beam_size);
  }

  double max_score = best_seqs[0].score;
  int selected = 0;
  for (int i = 1, sz = best_seqs.size(); i < sz; ++i) {
    if (best_seqs[i].score > max_score) {
      selected = i;
      max_score = best_seqs[i].score;
    }
  }
  return best_seqs[selected];
}

int main() {
  return 0;
}
