#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
using namespace std;

const int MAPWIDTH = 21;
const int MAPHEIGHT = 21;
const int BOSS_X = 10;
const int BOSS_Y = 10;

int playerX = 0, playerY = 0;
int playerHP = 250;


// -------- GAME STATE --------
bool hasKey = false;
bool bossUnlocked = false;
bool bossKeyFound = false; 
bool gameRunning = true;
bool hasAxe = false;
bool hasChainSaw = false;
bool hasFists = true;
bool hasKnife = false;
bool hasSword = false;
bool hasGun = false;
char choice;
int storedHealth = 0;
int bullets = 6;

const int HEALTH_PACK_HEAL = 30;

void typeText(const string& text, int delay = 30) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
}

void drawHealthBars(int enemyHP, int maxEnemyHP, int playerHP) {
    typeText("Enemy Health: ", 0);
    int enemyBars = (enemyHP * 10) / maxEnemyHP;
    for(int i = 0; i < 10; i++) {
        if(i < enemyBars) typeText("-", 0);
        else typeText(" ", 0);
    }
    typeText(" (" + to_string(enemyHP) + "/" + to_string(maxEnemyHP) + ")  Player Health: ", 0);
    int playerBars = (playerHP * 10) / 250;  // Assuming max player HP is 250 for bar display
    for(int i = 0; i < 10; i++) {
        if(i < playerBars) typeText("-", 0);
        else typeText(" ", 0);
    }
    typeText(" (" + to_string(playerHP) + ")\n", 0);
}

bool happens(int percent) {
    return (rand() % 100) < percent;
}

void offerStoredHealthPack(int damageTaken, int oldHP) {
    if (storedHealth <= 0) return;
    if (damageTaken < 15 && oldHP > 100) return;

    typeText("You lost " + to_string(damageTaken) + " HP. You have a stored health pack. Use it now? (y/n) ", 0);
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        storedHealth--;
        playerHP += HEALTH_PACK_HEAL;
        typeText("You used a stored health pack and recovered " + to_string(HEALTH_PACK_HEAL) + " HP.\n");
    } else {
        typeText("You keep the health pack for later.\n");
    }
}

string getCombatChoice() {
    string action;
    typeText("Choose action before the enemy attack (block/dodge/run/attack): ", 0);
    cin >> action;
    return action;
}

int fist() {
    int damage = 8;
    int roll = rand() % 100;

    if (roll < 5) {
        typeText("Critical hit with fists!\n");
        damage *= 1.5;
    }
    else if (roll < 15) {
        typeText("You hit twice!\n");
        damage *= 2;
    }

    return damage;
}

int knife() {
    int damage = 14;
    int roll = rand() % 100;

    if (roll < 15) {
        typeText("Critical hit with knife!\n");
        damage *= 1.5;
    }
    else if (roll < 40) {
        typeText("You hit twice!\n");
        damage *= 2;
    }

    return damage;
}

int sword() {
    int damage = 20;
    int roll = rand() % 100;

    if (roll < 10) {
        typeText("Critical hit with sword!\n");
        damage *= 1.5;
    }
    else if (roll < 40) {
        typeText("STAB, STAB, SLASH!\n");
        damage *= 2;
    }

    return damage;
}

int gun() {
    if (bullets <= 0) {
        typeText("Out of ammo! Reloading...\n");
        bullets = 6;
        return 0;
    }

    int damage = 18;
    int roll = rand() % 100;

    
    bullets--;

    if (roll < 10) {
        typeText("You missed!\n");
        return 0;
    }
    else if (roll < 30) {
        typeText("Critical hit with gun!\n");
        return damage * 1.5;
    }
    else if (roll < 45 && bullets >= 2) {
        typeText("RAPID FIRE!\n");
        bullets -= 2;
        return damage * 3;
    }

    return damage;
}

bool fightEnemy(int enemyHP, int enemyDamage, bool allowRun = true, bool isBoss = false, int maxEnemyHP = 100) {
    typeText("Combat started!\n");
    bool enemyDefeated = false;

    while (enemyHP > 0 && playerHP > 0) {
        int playerDamage = 0;

        if (hasGun) playerDamage = gun();
        else if (hasSword) playerDamage = sword();
        else if (hasKnife) playerDamage = knife();
        else playerDamage = fist();

        typeText("You deal " + to_string(playerDamage) + " damage.\n");
        enemyHP -= playerDamage;

        if (enemyHP <= 0) break;

        // Display health bars
        drawHealthBars(enemyHP, maxEnemyHP, playerHP);

        string action = getCombatChoice();
        bool escaped = false;
        bool avoided = false;
        int damageTaken = enemyDamage;
        
        // Boss special abilities
        if (isBoss && happens(30)) {
            int ability = rand() % 3;
            if (ability == 0) {
                damageTaken = enemyDamage * 2;
                typeText("The boss uses HEAVY STRIKE! Damage doubled!\n");
            } else if (ability == 1) {
                typeText("The boss uses POWER STANCE! Block effectiveness reduced!\n");
                damageTaken = enemyDamage + 5;
            } else {
                typeText("The boss uses REGENERATION! Heals 30 HP!\n");
                enemyHP += 30;
            }
        }

        if (action == "run") {
            if (!allowRun) {
                typeText("You can't run from the boss!\n");
            } else if (happens(40)) {
                typeText("You successfully run away from the enemy!\n");
                escaped = true;
            } else {
                typeText("Run attempt failed!\n");
            }
        }
        else if (action == "dodge") {
            if (happens(55)) {
                typeText("You dodge the enemy attack!\n");
                avoided = true;
            } else {
                typeText("You try to dodge but fail.\n");
            }
        }
        else if (action == "block") {
            if (happens(50)) {
                damageTaken = (damageTaken + 1) / 2;
                typeText("You block part of the damage and take only " + to_string(damageTaken) + "!\n");
            } else {
                typeText("Block failed and you take full damage.\n");
            }
        }
        else if (action == "attack") {
            typeText("You brace yourself for the enemy attack.\n");
        }

        if (escaped) {
            break;
        }

        if (!avoided) {
            int oldHP = playerHP;
            typeText("Enemy hits you for " + to_string(damageTaken) + "\n", 0);
            playerHP -= damageTaken;
            if (playerHP > 0) {
                offerStoredHealthPack(damageTaken, oldHP);
            }
        }
    }

    if (playerHP <= 0) {
        typeText("You died!\n");
        gameRunning = false;
    } else if (enemyHP <= 0) {
        typeText("Enemy defeated!\n");
        enemyDefeated = true;
    }

    return enemyDefeated;
}

string randomRoom() {
    int roll = rand() % 100;

    if (roll < 25) return "hallway";
    else if (roll < 43) return "patient";     // 18%
    else if (roll < 50) return "nursing";     // 7%
    else if (roll < 58) return "doctor";      // 8%
    else if (roll < 67) return "medical";     // 9%
    else if (roll < 77) return "bathroom";    // 10%
    else if (roll < 83) return "cafeteria";   // 6%
    else if (roll < 93) return "locked";      // 10%
    else return "boarded";                    // 7%
}


struct Room {
    string type;
    bool visited;
    bool used;       // loot taken
    bool locked;
};

Room mapGrid[MAPHEIGHT][MAPWIDTH];

void showStatus() {
    typeText("\nHP: " + to_string(playerHP), 0);
    typeText(" | Key: " + string(hasKey ? "Yes" : "No") + "\n", 0);
}

void cabinet() {
    typeText("You open the cabinet...\n");
    bool foundSomething = false;
    
    // Health pack - 25% chance
    if (happens(25)) {
        foundSomething = true;
        typeText("You found a health pack!\n");
        typeText("Do you want to use it? (y/n) ", 0);
        cin >> choice;
        if (choice == 'y') {
            playerHP += 20;
            typeText("You used the health pack. +20 HP.\n");
        } else {
            typeText("Might come in handy later...\n");
            storedHealth += 1;
        }
    }
    
    // Regular key - 15% chance
    if (happens(15)) {
        foundSomething = true;
        typeText("You found a key!\n");
        hasKey = true;
    }
    
    // Weapon - increased spawn rate to 40%
    if (happens(40)) {
        foundSomething = true;
        typeText("You found a weapon!\n");
        int rollWeapon = rand() % 100;
        if (rollWeapon < 30) {
            typeText("It's a knife.\n");
            typeText("Did you want to use it? (y/n) ", 0);
            cin >> choice;
            if (choice == 'y') {
                hasKnife = true;
                hasSword = false;
                hasGun = false;
                hasFists = false;
                typeText("You equip the knife.\n");
            } else {
                typeText("You leave the knife.\n");
            }
        } else if (rollWeapon < 65) {
            typeText("It's a sword.\n");
            typeText("Did you want to use it? (y/n) ", 0);
            cin >> choice;
            if (choice == 'y') {
                hasSword = true;
                hasKnife = false;
                hasGun = false;
                hasFists = false;
                typeText("You equip the sword.\n");
            } else {
                typeText("You leave the sword.\n");
            }
        } else {
            typeText("It's a gun.\n");
            typeText("Did you want to use it? (y/n) ", 0);
            cin >> choice;
            if (choice == 'y') {
                hasGun = true;
                hasKnife = false;
                hasSword = false;
                hasFists = false;
                typeText("You equip the gun.\n");
            } else {
                typeText("You leave the gun.\n");
            }
        }
    }
    
    // Heavy weapons - 20% chance
    if (happens(20)) {
        foundSomething = true;
        int rollBreaker = rand() % 100;
        if (rollBreaker < 50) {
            typeText("You found an axe!\n");
            typeText("Did you want to use it? (y/n) ", 0);
            cin >> choice;
            if (choice == 'y') {
                hasAxe = true;
                hasChainSaw = false;
            } else {
                typeText("You leave the axe.\n");
            }
        } else {
            typeText("You found a chainsaw!\n");
            typeText("Did you want to keep it? (y/n) ", 0);
            cin >> choice;
            if (choice == 'y') {
                hasChainSaw = true;
                hasAxe = false;
            } else {
                typeText("You leave the chainsaw.\n");
            }
        }
    }
    
    if (!foundSomething) {
        typeText("The cabinet is empty.\n");
    }
}

void describeRoom() {
    Room &room = mapGrid[playerY][playerX];

    if (room.visited) {
        typeText("You've been here before.\n");
    } else {
        typeText("This place feels unfamiliar...\n");
    }

    if (room.type == "hallway") {
        typeText("You are in a hallway\n");

        if (happens(25)) {
            typeText("with a cabinet.\n");
            typeText("Do you want to open it? (y/n) ", 0);
            char choice;
            cin >> choice;
            if (choice == 'y') {
                cabinet();
            } else {
                typeText("You ignore the cabinet.\n");
            }
        }

        if (happens(15)) {
            typeText("a small enemy appears!\n");
            fightEnemy(50, 10, true, false, 50);
        }
        if (happens(10)) {
            typeText("a medium enemy appears!\n");
            fightEnemy(125, 18, true, false, 125);
        }
        if (happens(5)) {
            typeText("a large enemy appears!\n");
            fightEnemy(200, 24, true, false, 200);
        }


    } else if (room.type == "patient") {
        typeText("Patient room. Strange medicine here.\n");
        
        if (!room.used) {
            typeText("Do you want to consume the strange medicine? (y/n) ", 0);
            cin >> choice;
            
            if (choice == 'y' || choice == 'Y') {
                int medicineRoll = rand() % 100;
                if (medicineRoll < 60) {
                    playerHP += 30;
                    typeText("The medicine was good! You gained 30 HP.\n");
                } else {
                    playerHP -= 20;
                    typeText("The medicine was bad! You lost 20 HP.\n");
                }
            } else {
                typeText("You wisely decide not to consume unknown medicine.\n");
            }
            room.used = true;
        }

        if (happens(25)) {
            typeText("a cabinet.\n");
            typeText("Do you want to open it? (y/n) ", 0);
            char choice;
            cin >> choice;
            if (choice == 'y') {
                cabinet();
            } else {
                typeText("You ignore the cabinet.\n");
            }
        }

        if (happens(15)) {
            typeText("a small enemy appears!\n");
            fightEnemy(50, 10, true, false, 50);
        }
        if (happens(10)) {
            typeText("a medium enemy appears!\n");
            fightEnemy(125, 18, true, false, 125);
        }
        if (happens(5)) {
            typeText("a large enemy appears!\n");
            fightEnemy(200, 24, true, false, 200);
        }
    } else if (room.type == "cafeteria") {
        typeText("Cafeteria. Food available.\n");

          if (happens(25)) {
            typeText("a cabinet.\n");
            typeText("Do you want to open it? (y/n) ", 0);
            char choice;
            cin >> choice;
            if (choice == 'y') {
                cabinet();
            } else {
                typeText("You ignore the cabinet.\n");
            }
        }

    } else if (room.type == "locked") {
        typeText("Locked room - A prep chamber for the boss fight.\n");
        if (!hasKey) {
            typeText("You need a key to enter.\n");
        } else {
            typeText("You use the key to enter. The air feels heavy here.\n");
            if (!room.used) {
                typeText("Do you want to prepare yourself? (y/n) ", 0);
                cin >> choice;
                if (choice == 'y') {
                    typeText("You search through the supplies and equipment...\n");
                    cabinet();
                    
                    // Find medical supplies
                    int medCount = 2 + (rand() % 3);  // 2-4 med packs
                    typeText("You find " + to_string(medCount) + " medical packs!\n");
                    storedHealth += medCount;
                    typeText("Stored health packs: " + to_string(storedHealth) + "\n");
                    
                    room.used = true;
                    
                    // Boss prep enemies
                    if (happens(60)) {
                        typeText("A guardian appears!\n");
                        fightEnemy(150, 20, true, false, 150);
                    }
                    
                    // Chance to find boss key (higher if not yet found)
                    if (!bossUnlocked) {
                        int keyChance = bossKeyFound ? 20 : 50;
                        if (happens(keyChance)) {
                            typeText("You found the boss key!\n");
                            bossUnlocked = true;
                            bossKeyFound = true;
                            mapGrid[BOSS_Y][BOSS_X].locked = false;
                        }
                    }
                } else {
                    typeText("You leave the prep chamber.\n");
                }
            } else {
                typeText("This prep chamber has already been cleared.\n");
            }
        }
    } else if (room.type == "doctor") {
        typeText("Doctor office.\n");
        typeText("You find a medical cabinet.\n");
        playerHP += 80;
        typeText("You heal yourself for 80 HP.\n");
        cabinet();
    } else if (room.type == "nursing") {
        typeText("Nursing office.\n");
        typeText("You find a medical cabinet.\n");
        playerHP += 40;
        typeText("You heal yourself for 40 HP.\n");
        cabinet();
    } else if (room.type == "medical") {
        typeText("Medical room.\n");
        typeText("You find a medical cabinet.\n");
        playerHP += 20;
        typeText("You heal yourself for 20 HP.\n");
        cabinet();
    } else if (room.type == "bathroom") {
        typeText("Bathroom.\n");
        if (happens(25)) {
            typeText("a cabinet.\n");
            typeText("Do you want to open it? (y/n) ", 0);
            cin >> choice;
            if (choice == 'y') {
                cabinet();
            } else {
                 typeText("You ignore the cabinet.\n");
             }
        }

            if (happens(15)) {
            typeText("a small enemy appears!\n");
            fightEnemy(50, 10, true, false, 50);
        }
        if (happens(10)) {
            typeText("a medium enemy appears!\n");
            fightEnemy(125, 18, true, false, 125);
        }
        if (happens(5)) {
            typeText("a large enemy appears!\n");
            fightEnemy(200, 24, true, false, 200);
        }
    } else if (room.type == "boss") {
        typeText("Boss room.\n");
        if (!bossUnlocked) {
            typeText("A powerful presence blocks your way. You need a key to enter.\n");
        } else if (!room.used) {
            bool bossDefeated = fightEnemy(300, 35, false, true, 300);
            room.used = true;
            if (bossDefeated) {
                typeText("\nYou have defeated the boss and escaped the facility!\n");
                gameRunning = false;
            } else if (playerHP <= 0) {
                gameRunning = false;
            }
        } else {
            typeText("The boss has already been defeated.\n");
        }
    } else if (room.type == "boarded") {
        typeText("Boarded up room.\n");
        if (hasAxe) {
            typeText("You break through with the axe!\n");
            hasAxe = false; // axe breaks after use
            typeText("Your axe breaks after use.\n");
        } else if (hasChainSaw) {
            typeText("You cut through with the chainsaw!\n");
        } else {
            typeText("You can't get through.\n");
        }   
    }
    room.visited = true;
}

void initializeMap() {
    // Initialize all rooms
    for (int y = 0; y < MAPHEIGHT; y++) {
        for (int x = 0; x < MAPWIDTH; x++) {
            mapGrid[y][x].visited = false;
            mapGrid[y][x].used = false;
            mapGrid[y][x].locked = false;
            
            // Create cross pattern with hallways leading to boss
            if ((y == BOSS_Y) || (x == BOSS_X)) {
                // Center row and column are hallways
                mapGrid[y][x].type = "hallway";
            } else if (x == 0 || x == MAPWIDTH - 1 || y == 0 || y == MAPHEIGHT - 1) {
                // Edge rooms are hallways
                mapGrid[y][x].type = "hallway";
            } else {
                // Interior rooms are random
                mapGrid[y][x].type = randomRoom();
                if (mapGrid[y][x].type == "locked") {
                    mapGrid[y][x].locked = true;
                }
            }
        }
    }

    // Force important rooms
    mapGrid[0][0].type = "hallway";   // start
    mapGrid[BOSS_Y][BOSS_X].type = "boss";
    mapGrid[BOSS_Y][BOSS_X].locked = false;
    
    // Ensure at least one locked room has the boss key
    vector<pair<int, int> > lockedRooms;
    for (int y = 0; y < MAPHEIGHT; y++) {
        for (int x = 0; x < MAPWIDTH; x++) {
            if (mapGrid[y][x].locked) {
                lockedRooms.push_back(make_pair(x, y));
            }
        }
    }
    
    if (!lockedRooms.empty()) {
        int randomLocked = rand() % lockedRooms.size();
        int bossKeyX = lockedRooms[randomLocked].first;
        int bossKeyY = lockedRooms[randomLocked].second;
        // Store this for use in cabinet
        // This will be marked specially when cabinet is opened in a locked room
        bossKeyFound = false;  // Will be set to true when found
    }
}

void movePlayer(string dir) {
    int newX = playerX;
    int newY = playerY;

    if (dir == "w") newY--;
    else if (dir == "s") newY++;
    else if (dir == "a") newX--;
    else if (dir == "d") newX++;

    if (newX < 0 || newX >= MAPWIDTH || newY < 0 || newY >= MAPHEIGHT) {
        typeText("You hit a wall.\n");
        return;
    }

    Room &nextRoom = mapGrid[newY][newX];

    if (nextRoom.locked && !hasKey) {
        typeText("The room is locked.\n");
        return;
    }

    playerX = newX;
    playerY = newY;

}

void interact() {
    Room &room = mapGrid[playerY][playerX];

    if (room.used) {
        typeText("Nothing left here.\n");
        return;
    }

    if (room.type == "patient") {
        typeText("You found some strange medicine.\n");
        typeText("Do you want to consume it? (y/n) ", 0);
        cin >> choice;
        
        if (choice == 'y' || choice == 'Y') {
            int medicineRoll = rand() % 100;
            if (medicineRoll < 60) {
                playerHP += 30;
                typeText("The medicine was good! You gained 30 HP.\n");
            } else {
                playerHP -= 20;
                typeText("The medicine was bad! You lost 20 HP.\n");
            }
        } else {
            typeText("You wisely decide not to consume unknown medicine.\n");
        }

        typeText("You found a key!\n");
        hasKey = true;
        room.used = true;
    }

    else if (room.type == "cafeteria") {
        typeText("There's fresh food available.\n");
        typeText("Do you want to eat? (y/n) ", 0);
        cin >> choice;
        
        if (choice == 'y' || choice == 'Y') {
            int foodQuality = rand() % 100;
            if (foodQuality < 70) {
                playerHP += 25;
                typeText("Delicious! You gained 25 HP.\n");
            } else {
                playerHP += 10;
                typeText("The food was okay. You gained 10 HP.\n");
            }
        } else {
            typeText("You decide not to eat.\n");
        }
        room.used = true;
    }

    else if (room.type == "locked") {
        typeText("You found the artifact!\n");
        bossUnlocked = true;

        // unlock boss room
        mapGrid[5][5].locked = false;
    }


}

void drawMap() {
    typeText("\nMap:\n", 0);

    for (int y = 0; y < MAPHEIGHT; y++) {
        for (int x = 0; x < MAPWIDTH; x++) {

            if (x == playerX && y == playerY) {
                typeText("P ", 0);
            }
            else if (!mapGrid[y][x].visited) {
                typeText("? ", 0);
            }
            else {
                // Show first letter of room type
                char symbol = toupper(mapGrid[y][x].type[0]);

                // Make boss room obvious
                if (mapGrid[y][x].type == "boss") symbol = 'B';

                typeText(string(1, symbol) + " ", 0);
            }
        }
        typeText("\n", 0);
    }
}

int main() {
    srand(time(0));
    initializeMap();

    mapGrid[playerY][playerX].visited = true;

    string input;

    while (gameRunning && playerHP > 0) {
        showStatus();
        drawMap();
        describeRoom();

        typeText("Move (w/a/s/d), interact, use medpack, or quit: ", 0);
        cin >> input;

        if (input == "w" || input == "a" || input == "s" || input == "d") {
            movePlayer(input);
        }
        else if (input == "interact") {
            interact();
        }
        else if (input == "use medpack") {
                if (storedHealth > 0) {
                    storedHealth--;
                    playerHP += HEALTH_PACK_HEAL;
                    typeText("You used a stored health pack and recovered " + to_string(HEALTH_PACK_HEAL) + " HP.\n");
                    typeText("Stored medpacks remaining: " + to_string(storedHealth) + "\n");
                } else {
                    typeText("You don't have any stored medpacks.\n");
                }

        }
        else if (input == "quit") {
            break;
        }
        else {
            typeText("Invalid command.\n");
        }
    }

    typeText("Game Over.\n");
    return 0;
}
