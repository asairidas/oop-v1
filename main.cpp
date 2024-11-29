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
#include <numeric>
#include <random>
#include <filesystem>

#include "mokinys.h"
#include "statistika.h"

using namespace std;
namespace fs = std::filesystem;

// Struktūros Laikai apibrėžimas
struct Laikai
{
    double nuskaitymo_laikas;
    double rikiavimo_laikas;
    double padalijimo_laikas;
    double protingu_irasymo_laikas;
    double silpnu_irasymo_laikas;
    double bendras_laikas;
};

/*
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
*/
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
/*
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
        // Mokinys m = nuskaityti_mokinio_duomenis();
        // mokiniai.push_back(m);
    }
    return mokiniai;
}
*/

vector<Mokinys> duomenu_nuskaitymas_is_failo(const string &failo_vardas)
{
    vector<Mokinys> mokiniai;

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

bool pagal_varda(const Mokinys &kairys, const Mokinys &desinys)
{
    return kairys.vardas < desinys.vardas;
}

bool pagal_pavarde(const Mokinys &kairys, const Mokinys &desinys)
{
    if (kairys.pavarde == desinys.pavarde)
    {
        return kairys.vardas < desinys.vardas;
    }
    return kairys.pavarde < desinys.pavarde;
}

bool pagal_galutini(const Mokinys &kairys, const Mokinys &desinys)
{
    return kairys.galutinis < desinys.galutinis;
}

// funkcija, kuri sugeneruos faila su atsitiktiniais duomenimis
void failu_generavimas(int eiluciu_skaicius, const string &failo_vardas)
{
    ofstream failas(failo_vardas);
    if (!failas)
    {
        cerr << "Nepavyko sukurti failo " << endl;
        return;
    }

    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> pazymiu_generavimas(1, 10);

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
            failas << pazymiu_generavimas(mt) << " ";
        }
        failas << pazymiu_generavimas(mt) << endl;
    }
}

void failu_irasymas(const vector<Mokinys> &mokiniai, const string &failo_vardas)
{
    ofstream failas(failo_vardas);
    if (!failas)
    {
        cerr << "Nepavyko sukurti failo " << failo_vardas << "." << endl;
        return;
    }

    // irasom pirma eilute
    failas << left << setw(15) << "Vardas" << setw(15) << " Pavarde" << "Galutinis" << endl;

    for (const auto &mokinys : mokiniai)
    {
        failas << left << setw(15) << mokinys.vardas << setw(15) << mokinys.pavarde << fixed << setprecision(2) << mokinys.galutinis << endl;
    }
}

Laikai eksperimentas(const string &failo_vardas, int eiluciu_kiekis, const string &rusiuoti_pagal)
{
    try
    {
        auto pradzia_nuskaitymas = chrono::high_resolution_clock::now();
        auto pradzia_bendras = pradzia_nuskaitymas;
        vector<Mokinys> mokiniai = duomenu_nuskaitymas_is_failo(failo_vardas);
        auto pabaiga_nuskaitymas = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> nuskaitymo_trukme = pabaiga_nuskaitymas - pradzia_nuskaitymas;
        cout << "Failo nuskaitymo laikas: " << nuskaitymo_trukme.count() << " ms" << endl;

        // kiekvieno mokinio galutinis pazymys bus skaiciuojamas pagal vidurki
        for (int i = 0; i < mokiniai.size(); i++)
        {
            mokiniai[i].galutinis = skaiciuoti_galutini(mokiniai[i], "vid");
        }

        auto pradzia_rusiavimas = chrono::high_resolution_clock::now();
        if (rusiuoti_pagal == "vardas")
        {
            sort(mokiniai.begin(), mokiniai.end(), pagal_varda);
        }
        else if (rusiuoti_pagal == "pavarde")
        {
            sort(mokiniai.begin(), mokiniai.end(), pagal_pavarde);
        }
        else if (rusiuoti_pagal == "galutinis")
        {
            sort(mokiniai.begin(), mokiniai.end(), pagal_galutini);
        }

        auto pabaiga_rusiavimas = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> rusiavimo_trukme = pabaiga_rusiavimas - pradzia_rusiavimas;
        cout << "Rusiavimo laikas: " << rusiavimo_trukme.count() << " ms" << endl;

        vector<Mokinys> protingi;
        vector<Mokinys> silpni_moksluose;

        auto pradzia_padalijimo = chrono::high_resolution_clock::now();
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
        mokiniai.clear();

        auto pabaiga_padalijimo = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> padalijimo_trukme = pabaiga_padalijimo - pradzia_padalijimo;
        cout << "Padalijimo laikas: " << padalijimo_trukme.count() << " ms" << endl;

        string protingu_failo_vardas = "protingi_" + to_string(eiluciu_kiekis) + ".txt";
        string silpnu_failo_vardas = "silpni_" + to_string(eiluciu_kiekis) + ".txt";

        auto pradzia_protingu_irasymas = chrono::high_resolution_clock::now();
        failu_irasymas(protingi, protingu_failo_vardas);
        auto pabaiga_protingu_irasymas = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> protingu_irasymas_trukme = pabaiga_protingu_irasymas - pradzia_protingu_irasymas;
        cout << "Protingu mokiniu isvedimo laikas: " << protingu_irasymas_trukme.count() << " ms" << endl;

        auto pradzia_silpnu_irasymas = chrono::high_resolution_clock::now();
        failu_irasymas(silpni_moksluose, silpnu_failo_vardas);
        auto pabaiga_silpnu_irasymas = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> silpnu_irasymas_trukme = pabaiga_silpnu_irasymas - pradzia_silpnu_irasymas;
        cout << "Silpnu mokiniu isvedimo laikas: " << silpnu_irasymas_trukme.count() << " ms" << endl;

        auto pabaiga_bendras = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> bendras_trukme = pabaiga_bendras - pradzia_bendras;
        cout << "Bendras laikas: " << bendras_trukme.count() << " ms" << endl;
        cout << endl;

        return {nuskaitymo_trukme.count(), rusiavimo_trukme.count(), padalijimo_trukme.count(), protingu_irasymas_trukme.count(), silpnu_irasymas_trukme.count(), bendras_trukme.count()};
    }
    catch (const exception &e)
    {
        cerr << "Klaida: " << e.what() << endl;
        return {0, 0, 0, 0, 0, 0};
    }
}

void generuoti_failus()
{
    vector<int> eksperimentai{1000, 10000, 100000, 1000000, 10000000};
    for (int i = 0; i < eksperimentai.size(); i++)
    {
        string failo_vardas = "sugeneruoti_duomenys" + to_string(eksperimentai[i]) + ".txt";
        if (!fs::exists(failo_vardas))
        {
            failu_generavimas(eksperimentai[i], failo_vardas);
        }
    }
}

int main()
{
    try
    {
        // Generuoju failus tik tuomet je jie neegzistuoja
        generuoti_failus();

        vector<int> eksperimentai{1000, 10000, 100000, 1000000, 10000000};
        int testu_kiekis = 2;

        string rusiuoti_pagal;
        while (true)
        {
            cout << "Pasirinkite pagal ka rusiuoti (varda, pavarde, galutini): ";
            cin >> rusiuoti_pagal;
            if (rusiuoti_pagal == "varda" || rusiuoti_pagal == "pavarde" || rusiuoti_pagal == "galutini")
            {
                break;
            }
            else
            {
                cout << "Neteisingas pasirinkimas. Bandykite dar kartą." << endl;
            }
        }
        for (int i = 0; i < eksperimentai.size(); i++)
        {

            vector<double> nuskaitymo_laikai;
            vector<double> rikiavimo_laikai;
            vector<double> padalijimo_laikai;
            vector<double> protingu_irasymo_laikai;
            vector<double> silpnu_irasymo_laikai;
            vector<double> bendri_laikai;
            vector<double> laiku_vektorius;

            cout << "---------[ Eksperimentai su " << eksperimentai[i] << " eiluciu ]---------" << endl;

            for (int j = 0; j < testu_kiekis; j++)
            {
                cout << "Testas " << j + 1 << ":" << endl;
                Laikai laikai = eksperimentas("sugeneruoti_duomenys" + to_string(eksperimentai[i]) + ".txt", eksperimentai[i], rusiuoti_pagal);
                nuskaitymo_laikai.push_back(laikai.nuskaitymo_laikas);
                rikiavimo_laikai.push_back(laikai.rikiavimo_laikas);
                padalijimo_laikai.push_back(laikai.padalijimo_laikas);
                protingu_irasymo_laikai.push_back(laikai.protingu_irasymo_laikas);
                silpnu_irasymo_laikai.push_back(laikai.silpnu_irasymo_laikas);
                bendri_laikai.push_back(laikai.bendras_laikas);
            }
            double vidutinis_nuskaitymo_laikas = accumulate(nuskaitymo_laikai.begin(), nuskaitymo_laikai.end(), 0.0) / nuskaitymo_laikai.size();
            double vidutinis_rikiavimo_laikas = accumulate(rikiavimo_laikai.begin(), rikiavimo_laikai.end(), 0.0) / rikiavimo_laikai.size();
            double vidutinis_padalijimo_laikas = accumulate(padalijimo_laikai.begin(), padalijimo_laikai.end(), 0.0) / padalijimo_laikai.size();
            double vidutinis_protingu_irasymo_laikas = accumulate(protingu_irasymo_laikai.begin(), protingu_irasymo_laikai.end(), 0.0) / protingu_irasymo_laikai.size();
            double vidutinis_silpnu_irasymo_laikas = accumulate(silpnu_irasymo_laikai.begin(), silpnu_irasymo_laikai.end(), 0.0) / silpnu_irasymo_laikai.size();
            double vidutinis_bendras_laikas = accumulate(bendri_laikai.begin(), bendri_laikai.end(), 0.0) / bendri_laikai.size();

            cout << "---------[ Vidutiniai laikai - " << eksperimentai[i] << " eiluciu ]---------" << endl;
            cout << "Vidutinis failo nuskaitymo laikas: " << vidutinis_nuskaitymo_laikas << " ms" << endl;
            cout << "Vidutinis rusiavimo laikas: " << vidutinis_rikiavimo_laikas << " ms" << endl;
            cout << "Vidutinis padalijimo i dvi grupes laikas: " << vidutinis_padalijimo_laikas << " ms" << endl;
            cout << "Vidutinis protingu mokiniu isvedimo i faila laikas: " << vidutinis_protingu_irasymo_laikas << " ms" << endl;
            cout << "Vidutinis silpnuju mokiniu isvedimo i faila laikas: " << vidutinis_silpnu_irasymo_laikas << " ms" << endl;
            cout << "Vidutinis bendras laikas: " << vidutinis_bendras_laikas << " ms" << endl;
            cout << endl;
        }
    }
    catch (const exception &e)
    {
        cerr << "Klaida pagrindinėje programoje: " << e.what() << endl;
    }
}