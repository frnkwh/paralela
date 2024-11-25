import numpy as np

def generate_random_vectors(n_elements, n_targets):
    # Gera um vetor de entrada aleatório
    input_array = np.random.randint(0, n_elements * 10, size=n_elements)
    input_array.sort()  # Ordena o vetor para busca binária

    # Gera um vetor de busca aleatório
    targets = np.random.randint(0, n_elements * 10, size=n_targets)

    return input_array, targets

def main():
    # Solicita o número de elementos e de buscas ao usuário
    n_elements = input()
    n_targets = input()

    # Gera os vetores
    input_array, targets = generate_random_vectors(n_elements, n_targets)
    print(input_array)

    #for e in input_array:
    #    print(e + " ")
    #for e in targets:
    #    print(e + " ")

if __name__ == "__main__":
    main()
