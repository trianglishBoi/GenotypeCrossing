#include <iostream>
#include <string>
#include <vector>
#include <array>

#include <cassert>

constexpr uint32_t GNumberOfAllelesInGenotype = 2;
constexpr uint32_t GNumberOfPossibleGenotypesAfterCrossing = 4;

constexpr int8_t GLowerUpperDiff = -32;
constexpr int8_t GNoDiff = 0;

namespace Utils
{
	const char* GetIntegerPostfix(size_t integer)
	{
		assert(integer != 0);
		return integer == 1 ? "st" : integer == 2 ? "nd" : integer == 3 ? "rd" : "th";
	}

	void RecoverCin()
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	void DoubleCinGet()
	{
		std::cin.get();
		std::cin.get();
	}
}

struct Allele
{
	char Symbol;

	Allele() : Symbol('\0') { }
	Allele(char symbol) : Symbol(symbol) { }

	bool IsDominant() const
	{
		return isupper(Symbol);
	}

	operator char() const
	{
		return Symbol;
	}
};

struct Genotype
{
	std::array<Allele, GNumberOfAllelesInGenotype> Alleles;

	Genotype() : Alleles{} { }
	Genotype(Allele allele0, Allele allele1) : Alleles{allele0, allele1} { }
	Genotype(const std::array<Allele, GNumberOfAllelesInGenotype>& alleles) : Alleles(alleles) { }

	bool IsFalselyOrdered() const
	{
		// xX check
		if (islower(Alleles[0]) && isupper(Alleles[1]))
		{
			return true;
		}

		return false;
	}

	bool IsValid() const
	{
		// Alleles must be the same characters. Examples: XX, Xx, xx
		char diff = Alleles[0] - Alleles[1];
		if (diff != GLowerUpperDiff && diff != GNoDiff)
		{
			return false;
		}

		return true;
	}

	bool CanBeCrossedWith(const Genotype& other) const
	{
		char diff0 = Alleles[0] - other[0];
		char diff1 = Alleles[1] - other[1];

		const bool chkdiff0 = diff0 != GLowerUpperDiff && diff0 != GNoDiff;
		const bool chkdiff1 = diff1 != GLowerUpperDiff && diff1 != GNoDiff;

		if (chkdiff0 || chkdiff1)
		{
			return false;
		}

		return true;
	}

	bool HasDominantCharacter() const
	{
		return Alleles[0].IsDominant();
	}

	bool IsPureDominant() const
	{
		return Alleles[0].IsDominant() ? Alleles[1].IsDominant() : false;
	}

	bool IsHeteregenousDominant() const
	{
		return Alleles[0].IsDominant() ? !Alleles[1].IsDominant() : false;
	}

	bool IsPure() const
	{
		return Alleles[0].IsDominant() ? Alleles[1].IsDominant() : true;
	}

	Allele& operator[](size_t index)
	{
		assert(index < GNumberOfAllelesInGenotype);
		return Alleles[index];
	}

	const Allele& operator[](size_t index) const
	{
		assert(index < GNumberOfAllelesInGenotype);
		return Alleles[index];
	}

	std::string ToString() const
	{
		std::string result;
		result.resize(2);

		result[0] = Alleles[0];
		result[1] = Alleles[1];

		return result;
	}

	operator std::string() const
	{
		return ToString();
	}

	operator bool() const
	{
		return IsValid();
	}
};

Genotype AskForGenotypeInput(int whichGenotype)
{
	const char* numPostfix = Utils::GetIntegerPostfix(whichGenotype);
	Genotype result;
	std::string input;

AskGenotype:
	std::cout << whichGenotype << numPostfix << " Genotype: ";

	if (!(std::cin >> input))
	{
		Utils::RecoverCin();

	ReAsk:
		std::cout << "Invalid genotype formatting, re-enter." << std::endl;
		goto AskGenotype;
	}

	if (input.size() != 2)
	{
		goto ReAsk;
	}

	result = Genotype(input[0], input[1]);
	if (!result)
	{
		goto ReAsk;
	}

	return result;
}

int main()
{
	Genotype firstGenotype = AskForGenotypeInput(1);
	Genotype secondGenotype = AskForGenotypeInput(2);

	if (!firstGenotype.CanBeCrossedWith(secondGenotype))
	{
		std::cout << "These genotypes cannot be crossed with each other!" << std::endl;
		Utils::DoubleCinGet();
		return -1;
	}

	// Separate all Q&A's from results.
	std::cout << "!-----------------------= POSSIBILITIES =-----------------------!" << std::endl;

	std::array<Genotype, GNumberOfPossibleGenotypesAfterCrossing> crossingResults =
	{
		Genotype(firstGenotype[0], secondGenotype[0]),
		Genotype(firstGenotype[0], secondGenotype[1]),
		Genotype(firstGenotype[1], secondGenotype[0]),
		Genotype(firstGenotype[1], secondGenotype[1])
	};

	size_t pureGenotypeCount{};
	size_t genotypesWithDominantCharacterCount{};

	size_t pureDominantGenotypeCount{};
	size_t heterogeneousDominantGenotypeCount{};

	for (size_t i = 1; i <= crossingResults.size(); i++)
	{
		Genotype& genotype = crossingResults[i - 1];

		// Was the result xX?
		if (genotype.IsFalselyOrdered())
		{
			std::swap(genotype[0].Symbol, genotype[1].Symbol);
		}

		if (genotype.IsPure())
		{
			pureGenotypeCount++;
		}

		if (genotype.IsPureDominant())
		{
			pureDominantGenotypeCount++;
		}
		else if (genotype.IsHeteregenousDominant())
		{
			heterogeneousDominantGenotypeCount++;
		}

		if (genotype.HasDominantCharacter())
		{
			genotypesWithDominantCharacterCount++;
		}

		std::cout << i << Utils::GetIntegerPostfix(i) << " possibility is " << genotype.ToString() << std::endl;
	}

	size_t heterogeneousGenotypeCount = GNumberOfPossibleGenotypesAfterCrossing - pureGenotypeCount;

	std::cout << "!-----------------------= OVERVIEW =-----------------------!" << std::endl;
	std::cout << "There was/were " << pureGenotypeCount << " pure genotypes from the possibilities." << std::endl;
	std::cout << "There was/were " << heterogeneousGenotypeCount << " heterogeneous genotypes from the possibilities." << std::endl;
	std::cout << "There was/were " << genotypesWithDominantCharacterCount << " genotypes with dominant character from the possibilities." << std::endl;

	std::cout << "!-----------------------= IN-DEPTH OVERVIEW =-----------------------!" << std::endl;
	std::cout << pureGenotypeCount << "/4 of the possibilities was/were pure." << std::endl;
	std::cout << heterogeneousGenotypeCount << "/4 of the possibilities was/were heterogeneous." << std::endl;
	std::cout << genotypesWithDominantCharacterCount << "/4 of the possibilities was/were dominant characters." << std::endl;
	std::cout << pureDominantGenotypeCount << "/4 of the possibilities was/were pure dominant characters." << std::endl;
	std::cout << heterogeneousDominantGenotypeCount << "/4 of the possibilities was/were heterogeneous dominant characters." << std::endl;

	Utils::DoubleCinGet();
}