#include <stdlib.h>
#include <vector>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence):m_condition(condition),m_sequence(sequence) {}

int NoeudInstTantQue::executer(){
    while(m_condition->executer()) m_sequence->executer();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* sequence, Noeud* condition):m_sequence(sequence), m_condition(condition) {}

int NoeudInstRepeter::executer(){
    for(int i = 1;i<=m_condition->executer();i++) m_sequence->executer();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* initialisation, Noeud* condition, Noeud* iteration, Noeud* sequence): m_initialisation(initialisation), m_condition(condition), m_iteration(iteration), m_sequence(sequence) {}

int NoeudInstPour::executer() {
    for(m_initialisation->executer();m_condition->executer();m_iteration->executer()) m_sequence->executer();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(Noeud* chaine): m_chaines() {
    this->ajoute(chaine);
}

int NoeudInstEcrire::executer() {
    string res;
    
        for (unsigned int i = 0; i < m_chaines.size(); i++) {
            if ( (typeid(*m_chaines[i])==typeid(SymboleValue) &&  *((SymboleValue*)m_chaines[i])== "<CHAINE>" )){
                res = res + ((SymboleValue*)m_chaines[i])->getChaine();
            } else {
                res = res + to_string(m_chaines[i]->executer());
            }
        }
        cout << res;
    return 0;
}

void NoeudInstEcrire::ajoute(Noeud * chaine){
    if (chaine!=nullptr) m_chaines.push_back(chaine);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(Noeud* variable) : m_variables() {
    this->ajoute(variable);
}

int NoeudInstLire::executer() {
        for (unsigned int i = 0; i < m_variables.size(); i++){
            int valeur;
            cin>>valeur;
            ((SymboleValue*)m_variables[i])->setValeur(valeur);
        }
        return 0;
}

void NoeudInstLire::ajoute(Noeud * variable){
    if (variable!=nullptr) m_variables.push_back(variable);
}

    
