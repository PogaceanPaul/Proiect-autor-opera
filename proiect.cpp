#include <string>
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <set>
#include <map>

using namespace std;

// Funcție care verifică dacă un caracter este literă capitală și nu face parte dintr-o secvență de litere capitale
bool isIsolatedUppercase(const wstring& line, size_t idx) {
    bool prevIsUpper = (idx > 0) && iswupper(line[idx - 1]);
    bool nextIsUpper = (idx < line.size() - 1) && iswupper(line[idx + 1]);
    return !prevIsUpper && !nextIsUpper;
}

// Seturi cu literele frecvente din fiecare limbă și ponderile lor extinse
map<wchar_t, int> romana = {
    {L'e', 25}, {L'a', 24}, {L'i', 23}, {L'r', 22}, {L't', 21}, {L'o', 20}, {L'n', 19}, {L'u', 18}, {L'l', 17}, {L's', 16},
    {L'd', 15}, {L'c', 14}, {L'm', 13}, {L'p', 12}, {L'b', 11}, {L'v', 10}, {L'g', 9}, {L'ș', 8}, {L'z', 7}, {L'ț', 6},
    {L'î', 5}, {L'â', 4}, {L'ă', 3}, {L'j', 2}, {L'x', 1}
};


map<wchar_t, int> english = {
    {L'e', 25}, {L't', 24}, {L'a', 23}, {L'o', 22}, {L'i', 21}, {L'n', 20}, {L's', 19}, {L'r', 18}, {L'h', 17}, {L'l', 16},
    {L'd', 15}, {L'c', 14}, {L'm', 13}, {L'f', 12}, {L'u', 11}, {L'p', 10}, {L'g', 9}, {L'b', 8}, {L'y', 7}, {L'v', 6},
    {L'k', 5}, {L'x', 4}, {L'j', 3}, {L'q', 2}, {L'z', 1}
};


map<wchar_t, int> deutsch = {
    {L'e', 25}, {L'n', 24}, {L'i', 23}, {L's', 22}, {L'r', 21}, {L'a', 20}, {L't', 19}, {L'u', 18}, {L'd', 17}, {L'h', 16},
    {L'g', 15}, {L'l', 14}, {L'k', 13}, {L'm', 12}, {L'z', 11}, {L'b', 10}, {L'p', 9}, {L'f', 8}, {L'v', 7}, {L'j', 6},
    {L'ä', 5}, {L'ö', 4}, {L'ü', 3}, {L'ß', 2}, {L'w', 1}
};


int calculateScore(const vector<wchar_t>& topLetters, const map<wchar_t, int>& languageSet) {
    int score = 0;
    for (const auto& ch : topLetters) {
        if (languageSet.find(ch) != languageSet.end()) {
            score += languageSet.at(ch);
        }
    }
    return score;
}

int main() {
    wstring line;

    ifstream fin("date.txt", ios::in | ios::binary);
    wofstream fout("out.txt");

    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    string utf8_line;

    unordered_map<wchar_t, int> freqMap;
    long long int tot = 0;
    bool in_word = false;

    wchar_t prev1 = L' ', prev2 = L' ';

    // Citim fiecare linie din fișierul de intrare
    while (getline(fin, utf8_line)) {
        line = converter.from_bytes(utf8_line);
        for (size_t i = 0; i < line.size(); ++i) {
            wchar_t ch = line[i];

            // Dacă e un spațiu sau punctuație, considerăm că ne aflăm la sfârșitul unui cuvânt
            if (iswspace(ch) || iswpunct(ch)) {
                in_word = false;
                prev2 = prev1;
                prev1 = ch;
                continue;
            }

            tot++;

            // Dacă litera este capitală și nu face parte dintr-o secvență de litere mari, o transformăm în mică
            if (iswupper(ch) && isIsolatedUppercase(line, i)) {
                ch = towlower(ch);
            }

            // Dacă am început un nou cuvânt sau propoziție
            if (!in_word) {
                freqMap[ch]++;
                in_word = true;  // Acum suntem în interiorul unui cuvânt
            } else {
                freqMap[ch]++;
            }

            prev2 = prev1;
            prev1 = ch;
        }
    }

    fout.imbue(locale(fout.getloc(), new codecvt_utf8<wchar_t>));

    // Transformăm map-ul într-un vector de perechi pentru sortare
    vector<pair<wchar_t, int>> freqVector(freqMap.begin(), freqMap.end());

    // Sortăm vectorul în funcție de frecvențe (descrescător)
    std::sort(freqVector.begin(), freqVector.end(), [](const pair<wchar_t, int>& a, const pair<wchar_t, int>& b) {
        return a.second > b.second;
    });

    for (const auto& p : freqVector) {
        fout << p.first << L" " << p.second << endl;
    }
    fout << endl;

    int count = 0;
    vector<wchar_t> top10Letters;  // Modificat pentru top 10 litere
    for (const auto& p : freqVector) {
        if (iswalpha(p.first)) { // Verificăm dacă este literă
            fout << p.first << L" " << p.second * 1.0 / tot * 100 << "%" << endl;
            top10Letters.push_back(p.first); // Salvăm literele în vector
            count++;
        }
        if (count >= 10) break; // Ne oprim după ce am afișat 10 litere
    }

    // Calculăm scorul pentru fiecare limbă
    int romanianScore = calculateScore(top10Letters, romana);
    int englishScore = calculateScore(top10Letters, english);
    int germanScore = calculateScore(top10Letters, deutsch);

    // Determinăm limba cu cel mai mare scor
    fout << L"Romanian Score: " << romanianScore << endl;
    fout << L"English Score: " << englishScore << endl;
    fout << L"German Score: " << germanScore << endl;

    if (romanianScore > englishScore && romanianScore > germanScore) {
        fout << L"ROMÂNĂ" << endl;
    } else if (englishScore > romanianScore && englishScore > germanScore) {
        fout << L"ENGLISH" << endl;
    } else if (germanScore > romanianScore && germanScore > englishScore) {
        fout << L"DEUTSCH" << endl;
    } else {
        fout << L"Limba detectată este ambiguă, scoruri egale." << endl;
    }

    return 0;
}
