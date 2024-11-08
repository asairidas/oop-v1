#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <iterator>
#include <chrono>
#include <stdexcept>

#include "mokinys.h"
#include "statistika.h"

using namespace std;

Mokinys nuskaityti_mokinio_duomenis()
{
    Mokinys m;
    cout << "Iveskite varda:" << endl;
    cin >> m.vardas;
    cout << "Ivedete varda " << m.vardas << endl;
    cout << "Iveskite pavarde" << endl;
    cin >> m.pavarde;
    cout << "Ivedete pavarde " << m.pavarde << endl;

    string str_pazymys;
    cout << "Iveskite pazymi (arba paspauskite Enter, kad baigtumete). Jei ivesite 0, bus sugeneruotas atsitiktinis balas: ";
    cin.ignore(10, '\n'); // Ignoruojame likusią eilutę po paskutinio cin
    while (getline(cin, str_pazymys) && !str_pazymys.empty())
    {
        try
        {
            int pazymys = stoi(str_pazymys);
            // jei pazymys yra 0 tai pakeiciame ji atsitiktiniu skaiciumi is intervalo [1, 10]
            if (pazymys == 0)
            {
                pazymys = rand() % 10 + 1;
                cout << "Sugeneruotas atsitiktinis pazymys: " << pazymys << endl;
            }
            m.namu_darbu_rezultatai.push_back(pazymys);
        }
        catch (const invalid_argument &e)
        {
            cerr << "Neteisingas įvesties formatas. Prašome įvesti skaičių." << endl;
        }
        catch (const out_of_range &e)
        {
            cerr << "Įvestas skaičius yra per didelis." << endl;
        }
        cout << "Iveskite pazymi (arba paspauskite Enter, kad baigtumete): ";
    }

    cout << "Iveskite egzamino rezultata. Jei ivesite 0, bus sugeneruotas atsitiktinis balas:" << endl;
    cin >> m.egzamino_rezultatas;
    // jei pazymys yra 0 tai pakeiciame ji atsitiktiniu skaiciumi is intervalo [1, 10]
    if (m.egzamino_rezultatas == 0)
    {
        m.egzamino_rezultatas = rand() % 10 + 1;
        cout << "Sugeneruotas atsitiktinis pazymys: " << m.egzamino_rezultatas << endl;
    }

    cout << "Ivedete egzamino rezultata " << m.egzamino_rezultatas << endl;

    return m;
}

double skaiciuoti_galutini(Mokinys &m, const string &pasirinkimas)
{
    double vid_med = 0;
    if (pasirinkimas == "med")
    {
        vid_med = skaiciuoti_mediana(m);
    }
    else
    {
        vid_med = skaiciuoti_vidurki(m);
    }
    double galutinis = 0.4 * vid_med + 0.6 * m.egzamino_rezultatas;
    return galutinis;
}

vector<Mokinys> duomenu_nuskaitymas_is_klaviaturos()
{
    int studentu_skaicius(0);
    while (true)
    {
        cout << "Iveskite studentu skaiciu" << endl;
        cin >> studentu_skaicius;
        if (studentu_skaicius <= 0)
        {
            cerr << "Studentų skaičius turi būti teigiamas sveikasis skaičius." << endl;
            continue; // prasykime vartotoja ivesti teisinga skaiciu
        }
        else
        {
            break; // jei skaicius teigiamas, nutraukime cikla
        }
    }

    vector<Mokinys> mokiniai;

    for (int k = 0; k < studentu_skaicius; ++k)
    {
        Mokinys m = nuskaityti_mokinio_duomenis();
        mokiniai.push_back(m);
    }
    return mokiniai;
}

vector<Mokinys> duomenu_nuskaitymas_is_failo(string failo_vardas)
{
    vector<Mokinys> mokiniai;

    // nuskaitome likusias eilutes ir sudedame i mokinio struktura
    // pridedame mokini i mokiniu vektoriu

    // atidarome faila
    ifstream ivedimo_failas(failo_vardas);
    if (!ivedimo_failas)
    {
        cerr << "Nepavyko atidaryti failo " << failo_vardas << endl;
        return mokiniai;
    }

    string eilute;
    getline(ivedimo_failas, eilute);
    // kad galetume naudoti iteratoriu reikia paversti stringa i istringstream
    istringstream iss(eilute);
    // nuskaitome pirma eilute ir suskaiciuojame kiek joje yra zodziu atskirtu tarpais, pagal tai nustatome kiek namu darbu irasu yra

    int namu_darbu_kiekis = 0;
    string zodis;
    while (iss >> zodis)
    {
        namu_darbu_kiekis++;
    }
    namu_darbu_kiekis -= 3; // atimame vardą, pavardę ir egzamino rezultatą

    while (getline(ivedimo_failas, eilute))
    {
        istringstream iss(eilute);
        Mokinys m;
        iss >> m.vardas >> m.pavarde;
        for (int i = 0; i < namu_darbu_kiekis; ++i)
        {
            int pazymys;
            iss >> pazymys;
            m.namu_darbu_rezultatai.push_back(pazymys);
        }
        iss >> m.egzamino_rezultatas;
        mokiniai.push_back(m);
    }

    ivedimo_failas.close();
    return mokiniai;
}

bool mokiniu_palygintojas(const Mokinys &kairys, const Mokinys &desinys)
{
    if (kairys.pavarde == desinys.pavarde)
    {
        return kairys.vardas < desinys.vardas;
    }
    return kairys.pavarde < desinys.pavarde;
}

// sukursiu funkcija, kuri sugeneruos faila su atsitiktiniais duomenimis
void failu_generavimas(int eiluciu_skaicius)
{
    ofstream failas("mokiniai.txt");
    if (!failas)
    {
        cerr << "Nepavyko sukurti failo mokiniai.txt" << endl;
        return;
    }

    // irasom pirma eilute
    failas << "Vardas Pavarde ";
    for (int i = 1; i <= 15; i++)
    {
        failas << "ND" << i << " ";
    }
    failas << "Egz." << endl;

    for (int i = 0; i < eiluciu_skaicius; i++)
    {
        failas << "vardas" << i << " pavarde" << i << " ";
        for (int j = 0; j < 15; j++)
        {
            failas << rand() % 10 + 1 << " ";
        }
        failas << rand() % 10 + 1 << endl;
    }
}

void failu_irasymas(vector<Mokinys> mokiniai, string failo_vardas)
{
    ofstream failas(failo_vardas);
    if (!failas)
    {
        cerr << "Nepavyko sukurti failo " << failo_vardas << "." << endl;
        return;
    }

    // irasom pirma eilute
    failas << "Vardas Pavarde ";

    auto pazymiu_kiekis = mokiniai[0].namu_darbu_rezultatai.size();

    for (int i = 1; i <= pazymiu_kiekis; i++)
    {
        failas << "ND" << i << " ";
    }
    failas << "Egz." << endl;

    for (auto mokinys : mokiniai)
    {
        failas << mokinys.vardas << " " << mokinys.pavarde << " ";
        for (auto pazymys : mokinys.namu_darbu_rezultatai)
        {
            failas << pazymys << " ";
        }
        failas << mokinys.egzamino_rezultatas << endl;
    }
}

void eksperimentas(int eiluciu_kiekis)
{
    try
    {
        failu_generavimas(eiluciu_kiekis);
        vector<Mokinys> mokiniai;

        auto pradzia_nuskaitymas = chrono::high_resolution_clock::now();
        auto pradzia_bendras = pradzia_nuskaitymas;
        mokiniai = duomenu_nuskaitymas_is_failo("mokiniai.txt");
        auto pabaiga_nuskaitymas = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> nuskaitymo_trukme = pabaiga_nuskaitymas - pradzia_nuskaitymas;

        auto pradzia_rikiavimas = chrono::high_resolution_clock::now();
        sort(mokiniai.begin(), mokiniai.end(), mokiniu_palygintojas);
        auto pabaiga_rikiavimas = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> rikiavimo_trukme = pabaiga_rikiavimas - pradzia_rikiavimas;

        // kiekvieno mokinio galutinis pazymys bus skaiciuojamas pagal vidurki

        auto pradzia_galutinio_skaiciavimas = chrono::high_resolution_clock::now();
        for (int i = 0; i < mokiniai.size(); i++)
        {
            mokiniai[i].galutinis = skaiciuoti_galutini(mokiniai[i], "vid");
        }
        auto pabaiga_galutinio_skaiciavimas = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> galutinio_skaiciavimo_trukme = pabaiga_galutinio_skaiciavimas - pradzia_galutinio_skaiciavimas;

        vector<Mokinys> protingi;
        vector<Mokinys> silpni_moksluose;

        for (int i = 0; i < mokiniai.size(); i++)
        {
            if (mokiniai[i].galutinis >= 5)
            {
                protingi.push_back(mokiniai[i]);
            }
            else
            {
                silpni_moksluose.push_back(mokiniai[i]);
            }
        }
        auto pradzia_protingu_irasymas = chrono::high_resolution_clock::now();
        failu_irasymas(protingi, "protingi.txt");
        auto pabaiga_protingu_irasymas = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> protingu_irasymas_trukme = pabaiga_protingu_irasymas - pradzia_protingu_irasymas;

        auto pradzia_silpnu_irasymas = chrono::high_resolution_clock::now();
        failu_irasymas(silpni_moksluose, "silpni_moksluose.txt");
        auto pabaiga_silpnu_irasymas = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> silpnu_irasymas_trukme = pabaiga_silpnu_irasymas - pradzia_silpnu_irasymas;

        auto pabaiga_bendras = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> bendras_trukme = pabaiga_bendras - pradzia_bendras;

        cout << "---------[ Programos greicio analize - " << eiluciu_kiekis << " eiluciu ]---------" << endl
             << endl;
        cout << "failo nuskaitymas uztruko: " << nuskaitymo_trukme.count() << " ms" << endl;
        cout << "rikiavimas uztruko: " << rikiavimo_trukme.count() << " ms" << endl;
        cout << "galutiniu pazymiu skaiciavimas uztruko: " << galutinio_skaiciavimo_trukme.count() << " ms" << endl;
        cout << "silpnuju mokiniu isvedimas i faila uztruko: " << silpnu_irasymas_trukme.count() << " ms" << endl;
        cout << "protingu mokiniu isvedimas i faila uztruko: " << protingu_irasymas_trukme.count() << " ms" << endl;
        cout << "visa trukme: " << bendras_trukme.count() << " ms" << endl;

        cout << endl
             << endl;
    }
    catch (const exception &e)
    {
        cerr << "Klaida: " << e.what() << endl;
    }
}

int main()
{
    try
    {
        vector<int> eksperimentai{1000, 10000, 100000, 1000000, 10000000};

        for (auto eksperimento_dydis : eksperimentai)
        {
            eksperimentas(eksperimento_dydis);
        }
    }
    catch (const exception &e)
    {
        cerr << "Klaida pagrindinėje programoje: " << e.what() << endl;
    }
}