#!/bin/bash

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${YELLOW}🛠️  Compilation...${NC}"
make > /dev/null

mkdir -p tests/actual # Dossier temporaire pour les résultats

TOTAL=0
PASSED=0

echo -e "${YELLOW}🧪 Lancement de la suite de tests...${NC}"

# On boucle sur tous les fichiers dans tests/inputs/
for input_file in tests/inputs/*.txt; do
    # On extrait le nom du fichier (ex: 01_basics.txt)
    base_name=$(basename "$input_file")
    
    expected_file="tests/expected/$base_name"
    actual_file="tests/actual/$base_name"

    # Si le fichier expected n'existe pas encore, on prévient et on passe
    if [ ! -f "$expected_file" ]; then
        echo -e "${YELLOW}⚠️  Ignoré : $base_name (Pas de fichier expected correspondant)${NC}"
        continue
    fi

    TOTAL=$((TOTAL + 1))

    # On lance le programme. 
    # Le '2>&1' redirige les erreurs (stderr) vers la sortie normale (stdout) 
    # pour pouvoir tester les messages d'erreur de ton try/catch !
    ./computorv2 < "$input_file" > "$actual_file" 2>&1

    # On compare
    diff -u "$expected_file" "$actual_file" > "tests/actual/${base_name}.diff"

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ PASS : $base_name${NC}"
        PASSED=$((PASSED + 1))
        rm "tests/actual/${base_name}.diff" # On nettoie si c'est bon
    else
        echo -e "${RED}❌ FAIL : $base_name${NC}"
    fi
done

echo "---------------------------------"
if [ $PASSED -eq $TOTAL ]; then
    echo -e "${GREEN}🎉 SUCCÈS : $PASSED / $TOTAL tests réussis !${NC}"
else
    echo -e "${RED}⚠️  ÉCHEC : $PASSED / $TOTAL tests réussis.${NC}"
    echo "Consulte les fichiers .diff dans le dossier tests/actual/ pour voir les erreurs."
fi