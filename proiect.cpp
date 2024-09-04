#include <string>
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cwctype>

using namespace std;

int main() {
    wstring line;

    ifstream fin("date.txt", ios::in | ios::binary);
    wofstream fout("out.txt");

    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    string utf8_line;

    unordered_map<wchar_t, int> freqMap;
    long long int tot = 0;
    wchar_t prev1='0', prev2='0';
    while (getline(fin, utf8_line)) {
        bool ok = 0;
        line = converter.from_bytes(utf8_line);
        for (wchar_t ch : line) {
            tot++;
            ///if((prev2 == '.' && (prev1 == ' ' || prev1 == '\n' || prev1 == '\0')) || ((prev2=='-' || prev2=='\'') && prev1 == ' ')|| prev1=='"' || prev1=='\n' || prev1 =='\0')
            if(prev2 == '.' || prev2 == '\'' || prev2=='"'|| prev2 =='\0' || prev2 =='\n' ||prev1 == '.' || prev1 == '\'' || prev1=='"'|| prev1 =='\0' || prev1 =='\n') 
            {
                wchar_t lower_ch = towlower(ch);
                freqMap[lower_ch]++;
            }
            else if(ok == 0)
            {
                wchar_t lower_ch = towlower(ch);
                freqMap[lower_ch]++;
                ok = 1;
            }
            else 
                freqMap[ch]++;
            prev2= prev1;
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
    fout<<endl;
    
    int count = 0;
    for (const auto& p : freqVector) {
        if (iswalpha(p.first)) { // Verificăm dacă este literă
            fout << p.first << L" " << p.second * 1.0 / tot * 100 <<"%" << endl;
            count++;
        }
        if (count >= 5) break; // Ne oprim după ce am afișat 5 litere
    }

    return 0;
}
