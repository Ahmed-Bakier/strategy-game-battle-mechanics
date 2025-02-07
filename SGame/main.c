//AHMED MOSTAFA BAKIER ABDELAZIZ 220201141
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#define MAX_LINE_LENGTH 1024
#define MAX_UNITS 10
#define MAX_HEROES 10
#define MAX_RESEARCH 10
#define MAX_CREATURES 10

typedef struct {
    char name[50];
    int attack;
    int defense;
    int health;
    float critical_chance;
    int max_health;
} Unit;

typedef struct {
    char name[50];
    float attack_bonus;
    float defense_bonus;
} Hero;

typedef struct {
    char name[50];
    float attack_bonus;
    float defense_penalty;
} Creature;

typedef struct {
    char name[50];
    int attack_level;
    int defense_level;
} Research;

Unit human_units[MAX_UNITS];
Unit orc_units[MAX_UNITS];
Hero heroes[MAX_HEROES];
Creature creatures[MAX_CREATURES];
Research research;

int num_human_units = 0;
int num_orc_units = 0;
int num_heroes = 0;
int num_creatures = 0;

//new
void remove_dead_units(Unit* units, int* num_units);

// URL
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    strcat(userp, contents);
    return size * nmemb;
}

// JSON verilerini indirme
int download_json(const char *url, char *data) {
    CURL *curl;
    CURLcode res;
    int success = 1;  // Başarı takibi için değişken

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, data);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "JSON verisi indirilirken hata: %s\n", curl_easy_strerror(res));
            success = 0;
        }

        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "CURL başlatılamadı.\n");
        success = 0;
    }

    curl_global_cleanup();


    if (strlen(data) == 0) {
        printf("Seçilen bağlantıda veri yok.\n");
        success = 0;
    }

    return success;
}


void save_json_to_file(const char *data, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Dosya yazmak için açılamadı: %s\n", filename);
        return;
    }

    fprintf(file, "%s", data);
    fclose(file);
    printf("JSON verisi %s dosyasına kaydedildi.\n", filename);
}


void load_units(const char *filename, Unit units[], int *num_units) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Hata: '%s' dosyası bulunamadı.\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (*num_units >= MAX_UNITS) break;
        sscanf(line, "%[^,],%d,%d,%d,%f",
               units[*num_units].name,
               &units[*num_units].attack,
               &units[*num_units].defense,
               &units[*num_units].max_health,
               &units[*num_units].critical_chance);
        units[*num_units].health = units[*num_units].max_health; // Başlangıç  sağlığını ayarla
        (*num_units)++;
    }
    fclose(file);
}


void load_heroes(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Hata: '%s' dosyası bulunamadı.\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (num_heroes >= MAX_HEROES) break;
        sscanf(line, "%[^,],%f,%f",
               heroes[num_heroes].name,
               &heroes[num_heroes].attack_bonus,
               &heroes[num_heroes].defense_bonus);
        num_heroes++;
    }
    fclose(file);
}


void load_creatures(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Hata: '%s' dosyası bulunamadı.\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (num_creatures >= MAX_CREATURES) break;
        sscanf(line, "%[^,],%f,%f",
               creatures[num_creatures].name,
               &creatures[num_creatures].attack_bonus,
               &creatures[num_creatures].defense_penalty);
        num_creatures++;
    }
    fclose(file);
}


void load_research(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Hata: '%s' dosyası bulunamadı.\n", filename);
        exit(EXIT_FAILURE);
    }

    fscanf(file, "%[^,],%d,%d",
           research.name,
           &research.attack_level,
           &research.defense_level);
    fclose(file);
}


float calculate_damage(Unit *attacker, Unit *defender) {
    float critical_multiplier = (rand() % 100 < attacker->critical_chance) ? 1.5 : 1.0;
    float damage = (attacker->attack * critical_multiplier) - defender->defense;
    return (damage < 0) ? 0 : damage;
}


void print_units(const char *faction, Unit *units, int num_units) {
    printf("\n%s Birimi Durumu:\n", faction);
    for (int i = 0; i < num_units; i++) {
        printf("Birim Adı: %s, Sağlık: %d/%d, Saldırı: %d, Savunma: %d, Kritik Vuruş Şansı: %.2f%%\n",
               units[i].name, units[i].health, units[i].max_health, units[i].attack, units[i].defense, units[i].critical_chance);
    }
}


void print_heroes() {
    printf("\nKahramanlar:\n");
    for (int i = 0; i < num_heroes; i++) {
        printf("Kahraman Adı: %s, Saldırı Artışı: %.2f, Savunma Artışı: %.2f\n",
               heroes[i].name, heroes[i].attack_bonus, heroes[i].defense_bonus);
    }
}


void print_creatures() {
    printf("\nMahluklar:\n");
    for (int i = 0; i < num_creatures; i++) {
        printf("Mahluk Adı: %s, Saldırı Artışı: %.2f, Savunma Ceza: %.2f\n",
               creatures[i].name, creatures[i].attack_bonus, creatures[i].defense_penalty);
    }
}


void print_research() {
    printf("\nAraştırma:\n");
    printf("Araştırma Adı: %s, Saldırı Seviyesi: %d, Savunma Seviyesi: %d\n",
           research.name, research.attack_level, research.defense_level);
}

float calculate_critical_hit(Unit *attacker) {
    return (rand() % 100 < attacker->critical_chance) ? 1.5 : 1.0;
}

void apply_fatigue(Unit *unit, int turn_count) {
    if (turn_count % 5 == 0) {  // Apply fatigue every 5 turns
        unit->attack *= 0.9;
        unit->defense *= 0.9;
    }
}

void apply_hero_bonus(Unit *units, int num_units, Hero *hero) {
    for (int i = 0; i < num_units; i++) {
        units[i].attack += units[i].attack * hero->attack_bonus;
        units[i].defense += units[i].defense * hero->defense_bonus;
    }
}

void apply_research_bonus(Unit *units, int num_units, Research *research) {
    float attack_multiplier = 1 + (research->attack_level * 0.05);
    float defense_multiplier = 1 + (research->defense_level * 0.05);

    for (int i = 0; i < num_units; i++) {
        units[i].attack *= attack_multiplier;
        units[i].defense *= defense_multiplier;
    }
}

void apply_creature_bonus(Unit *units, int num_units, Creature *creature) {
    for (int i = 0; i < num_units; i++) {
        units[i].attack += units[i].attack * creature->attack_bonus;
        units[i].defense -= units[i].defense * creature->defense_penalty;
    }
}
// new
void save_battle_log(const char *log) {
    FILE *file = fopen("battle_log.txt", "a"); // Open the file in append mode
    if (file == NULL) {
        fprintf(stderr, "Hata: Dosya yazmak için açılamadı: battle_log.txt\n");
        return;
    }
    fprintf(file, "%s\n", log); // Write the log to the file
    fclose(file);
}

// JSON verilerini
void load_scenario(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Hata: '%s' dosyası bulunamadı.\n", filename);
        exit(EXIT_FAILURE);
    }

    // structured JSON file
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Load human units
        if (strstr(line, "\"insan_imparatorlugu\"")) {
            while (fgets(line, sizeof(line), file) && !strstr(line, "\"ork_legi\"")) {
                sscanf(line, "\"%[^\"]\":%d,%d,%d,%f",
                       human_units[num_human_units].name,
                       &human_units[num_human_units].attack,
                       &human_units[num_human_units].defense,
                       &human_units[num_human_units].max_health,
                       &human_units[num_human_units].critical_chance);
                human_units[num_human_units].health = human_units[num_human_units].max_health;
                num_human_units++;
            }
        }

        // Load orc units
        if (strstr(line, "\"ork_legi\"")) {
            while (fgets(line, sizeof(line), file) && !strstr(line, "\"kahramanlar\"")) {
                sscanf(line, "\"%[^\"]\":%d,%d,%d,%f",
                       orc_units[num_orc_units].name,
                       &orc_units[num_orc_units].attack,
                       &orc_units[num_orc_units].defense,
                       &orc_units[num_orc_units].max_health,
                       &orc_units[num_orc_units].critical_chance);
                orc_units[num_orc_units].health = orc_units[num_orc_units].max_health;
                num_orc_units++;
            }
        }

        // Load heroes
        if (strstr(line, "\"kahramanlar\"")) {
            while (fgets(line, sizeof(line), file) && !strstr(line, "\"creatures\"")) {
                sscanf(line, "\"%[^\"]\":%f,%f",
                       heroes[num_heroes].name,
                       &heroes[num_heroes].attack_bonus,
                       &heroes[num_heroes].defense_bonus);
                num_heroes++;
            }
        }

        // Load creatures
        if (strstr(line, "\"creatures\"")) {
            while (fgets(line, sizeof(line), file)) {
                sscanf(line, "\"%[^\"]\":%f,%f",
                       creatures[num_creatures].name,
                       &creatures[num_creatures].attack_bonus,
                       &creatures[num_creatures].defense_penalty);
                num_creatures++;
            }
        }
    }
    fclose(file);
}

// Savaş simülasyon fonksiyonu
void battle() {
    // Kahraman ve araştırma
    apply_hero_bonus(human_units, num_human_units, &heroes[0]);
    apply_hero_bonus(orc_units, num_orc_units, &heroes[1]);
    apply_research_bonus(human_units, num_human_units, &research);
    apply_research_bonus(orc_units, num_orc_units, &research);
    apply_creature_bonus(human_units, num_human_units, &creatures[0]);
    apply_creature_bonus(orc_units, num_orc_units, &creatures[1]);

    // Savaş döngüsü
    while (num_human_units > 0 && num_orc_units > 0) {
        print_units("İnsan İmparatorluğu", human_units, num_human_units);
        print_units("Ork Lejyonu", orc_units, num_orc_units);

        // İnsan
        for (int i = 0; i < num_human_units; i++) {
            if (human_units[i].health <= 0) continue;

            for (int j = 0; j < num_orc_units; j++) {
                if (orc_units[j].health <= 0) continue;

                float damage = calculate_damage(&human_units[i], &orc_units[j]);
                orc_units[j].health -= damage;
                char log[200]; // Log için tampon
                snprintf(log, sizeof(log), "İnsan birimi '%s', Ork birimine '%s' %.2f hasar verdi.",
                         human_units[i].name, orc_units[j].name, damage);
                save_battle_log(log); // Log kaydet
                printf("%s\n", log); // Konsola yazdır

                if (orc_units[j].health <= 0) {
                    printf("Ork birimi '%s' yok oldu!\n", orc_units[j].name);
                    snprintf(log, sizeof(log), "Ork birimi '%s' yok oldu!", orc_units[j].name);
                    save_battle_log(log);
                }
                break;
            }
        }


        for (int i = 0; i < num_orc_units; i++) {
            if (orc_units[i].health <= 0) continue;

            for (int j = 0; j < num_human_units; j++) {
                if (human_units[j].health <= 0) continue;

                float damage = calculate_damage(&orc_units[i], &human_units[j]);
                human_units[j].health -= damage;
                char log[200];
                snprintf(log, sizeof(log), "Ork birimi '%s', İnsan birimine '%s' %.2f hasar verdi.",
                         orc_units[i].name, human_units[j].name, damage);
                save_battle_log(log);
                printf("%s\n", log);

                if (human_units[j].health <= 0) {
                    printf("İnsan birimi '%s' yok oldu!\n", human_units[j].name);
                    snprintf(log, sizeof(log), "İnsan birimi '%s' yok oldu!", human_units[j].name);
                    save_battle_log(log);
                }
                break; // Saldırıdan sonra bir sonraki saldırıya geç
            }
        }

        // Yorgunluk
        for (int i = 0; i < num_human_units; i++) {
            apply_fatigue(&human_units[i], i);
        }

        for (int i = 0; i < num_orc_units; i++) {
            apply_fatigue(&orc_units[i], i);
        }

        // Sağlığı sıfır olan
        remove_dead_units(human_units, &num_human_units);
        remove_dead_units(orc_units, &num_orc_units);
    }

    // Savaş
    if (num_human_units > 0) {
        printf("İnsan İmparatorluğu kazandı!\n");
    } else {
        printf("Ork Lejyonu kazandı!\n");
    }
}

// Sağlık
void remove_dead_units(Unit* units, int* num_units) {
    int count = 0;
    for (int i = 0; i < *num_units; i++) {
        if (units[i].health > 0) {
            units[count++] = units[i];  // Sağlıklı birimi koru
        }
    }
    *num_units = count;  // Yeni birim sayısını güncelle
}
void initSDL();
// Ana fonksiyon
int main(void) {
    char data[10000] = ""; // Veri depolamak için dizi
    char url[256];
    printf("Savaş senaryosunu indirmek için 1 ile 10 arasında bir sayı seçin: ");
    int choice;
    scanf("%d", &choice);

    snprintf(url, sizeof(url), "https://yapbenzet.org.tr/%d.json", choice);


    if (download_json(url, data)) {

        save_json_to_file(data, "scenario.json");


        load_units("scenario.json", human_units, &num_human_units);
        load_heroes("heroes.json");
        load_creatures("creatures.json");
        load_research("research.json");
        load_scenario("scenario.json");



        print_units("İnsan İmparatorluğu", human_units, num_human_units);
        print_units("Ork Lejyonu", orc_units, num_orc_units);
        print_heroes();
        print_creatures();
        print_research();

        battle();
    } else {
        printf("Geçersiz seçim!\n");
    }
    return 0;
}