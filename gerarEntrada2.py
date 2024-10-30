import numpy as np

def generate_random_vectors(n_targets):
    # Gera um vetor de busca aleatório
    targets = np.random.randint(0, n_targets, size=n_targets)

    return targets

def main():
    # Solicita o número de elementos e de buscas ao usuário
    n_elements = int(input())
    n_targets = int(input())

    # Gera o vetor
    targets = generate_random_vectors(n_targets)

    # Exibe os resultados
    print(n_elements)
    print(n_targets)
    print(' '.join(map(str, targets)))

if __name__ == "__main__":
    main()
