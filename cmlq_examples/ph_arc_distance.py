import numpy as np

def arc_distance():
    N = 10000
    import numpy as np
    np.random.seed(0)
    theta_1, phi_1, theta_2, phi_2 = np.random.randn(N), np.random.randn(N), np.random.randn(N), np.random.randn(N)


    """
    Calculates the pairwise arc distance between all points in vector a and b.
    """
    temp = np.sin((theta_2-theta_1)/2)**2+np.cos(theta_1)*np.cos(theta_2)*np.sin((phi_2-phi_1)/2)**2
    distance_matrix = 2 * (np.arctan2(np.sqrt(temp),np.sqrt(1-temp)))
    return distance_matrix