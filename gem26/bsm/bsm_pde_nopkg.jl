# Enhanced Black-Scholes-Merton (BSM) Model for American Options using PDE
# Pure Julia implementation (No external packages required)

# Simple Linear Interpolation
function simple_interpolate(x_grid, y_grid, x_target)
    if x_target <= x_grid[1] return y_grid[1] end
    if x_target >= x_grid[end] return y_grid[end] end
    
    # Find the interval
    for i in 1:(length(x_grid)-1)
        if x_target >= x_grid[i] && x_target <= x_grid[i+1]
            # Linear interp
            t = (x_target - x_grid[i]) / (x_grid[i+1] - x_grid[i])
            return y_grid[i] * (1.0 - t) + y_grid[i+1] * t
        end
    end
    return y_grid[end]
end

function price_american_option_pde(type::String, S::Float64, K::Float64, r::Float64, sigma::Float64, T::Float64)
    # Grid parameters
    S_max = 3 * S
    N_s = 100 # spatial steps
    N_t = 100 # time steps
    dt = T / N_t
    dS = S_max / N_s
    
    s_grid = [i * dS for i in 0:N_s]
    v = zeros(N_s + 1)
    
    # Initial value (at maturity)
    if type == "call"
        for i in 1:(N_s + 1)
            v[i] = max(s_grid[i] - K, 0.0)
        end
    else
        for i in 1:(N_s + 1)
            v[i] = max(K - s_grid[i], 0.0)
        end
    end
    
    # Backward in time
    for j in 1:N_t
        v_next = copy(v)
        for i in 2:N_s
            delta = (v[i+1] - v[i-1]) / (2 * dS)
            gamma = (v[i+1] - 2 * v[i] + v[i-1]) / (dS^2)
            
            # BS operator
            theta = - (0.5 * sigma^2 * s_grid[i]^2 * gamma + r * s_grid[i] * delta - r * v[i])
            v_next[i] = v[i] - dt * theta
            
            # Early exercise condition for American options
            if type == "call"
                v_next[i] = max(v_next[i], s_grid[i] - K)
            else
                v_next[i] = max(v_next[i], K - s_grid[i])
            end
        end
        # Boundary conditions
        if type == "call"
            v_next[1] = 0.0
            v_next[N_s+1] = s_grid[N_s+1] - K * exp(-r * (j * dt))
        else
            v_next[1] = K * exp(-r * (j * dt))
            v_next[N_s+1] = 0.0
        end
        v = v_next
    end
    
    return simple_interpolate(s_grid, v, S)
end

function enhanced_bsm_american(ticker::String, type::String, duration::String, S::Float64, K::Float64, r::Float64, sigma::Float64)
    T = 0.0
    if duration == "weekly"
        T = 7.0 / 365.0
    elseif duration == "monthly"
        T = 30.0 / 365.0
    else
        T = 0.25
    end
    
    return price_american_option_pde(type, S, K, r, sigma, T)
end
