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

int main() {
    wstring line;

    ifstream fin("date.txt", ios::in | ios::binary);
    wofstream fout("out.txt");

    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    string utf8_line;

    unordered_map<wchar_t, int> freqMap;
    long long int tot = 0;
    bool in_word = false;  // Indică dacă suntem în interiorul unui cuvânt

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
    vector<wchar_t> top5Letters;  // Modificat pentru top 5 litere
    for (const auto& p : freqVector) {
        if (iswalpha(p.first)) { // Verificăm dacă este literă
            fout << p.first << L" " << p.second * 1.0 / tot * 100 << "%" << endl;
            top5Letters.push_back(p.first); // Salvăm literele în vector
            count++;
        }
        if (count >= 5) break; // Ne oprim după ce am afișat 5 litere
    }

    // Seturi cu literele frecvente din fiecare limbă și ponderile lor
    map<wchar_t, int> romana = {{L'e', 5}, {L'a', 4}, {L'i', 3}, {L'r', 2}, {L't', 1}, {L'o', 1}, {L'n', 1}};
    map<wchar_t, int> english = {{L'e', 5}, {L't', 4}, {L'a', 3}, {L'o', 2}, {L'i', 1}, {L'n', 1}, {L's', 1}};
    map<wchar_t, int> deutsch = {{L'e', 5}, {L'n', 4}, {L'i', 3}, {L's', 2}, {L'r', 1}, {L'a', 1}, {L't', 1}};

    // Calculăm scorul pentru fiecare limbă
    int romanianScore = 0, englishScore = 0, germanScore = 0;
    for (const auto& ch : top5Letters) {
        if (romana.find(ch) != romana.end()) romanianScore += romana[ch];
        if (english.find(ch) != english.end()) englishScore += english[ch];
        if (deutsch.find(ch) != deutsch.end()) germanScore += deutsch[ch];
    }

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
