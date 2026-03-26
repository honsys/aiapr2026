# Enhanced Black-Scholes-Merton (BSM) Model for American Options using PDE
# Implementation for Gem polyglot interop

using DifferentialEquations
using Interpolations

# American Option Pricing via PDE (Finite Difference Method)
# This function will be translated to Gem on-the-fly.

function price_american_option_pde(type::String, S::Float64, K::Float64, r::Float64, sigma::Float64, T::Float64)
    # Parameters for the PDE
    # dV/dt + 0.5 * sigma^2 * S^2 * d^2V/dS^2 + r * S * dV/dS - r * V = 0
    # For American options, V(S,t) >= max(K-S, 0) for Put or max(S-K, 0) for Call
    
    # Grid parameters
    S_max = 3 * S
    N_s = 100 # spatial steps
    N_t = 100 # time steps
    dt = T / N_t
    dS = S_max / N_s
    
    # Boundary conditions and initial value (at maturity)
    # V(S, T) = payoff(S)
    s_grid = collect(0:dS:S_max)
    v = zeros(N_s + 1)
    
    if type == "call"
        for i in 1:(N_s + 1)
            v[i] = max(s_grid[i] - K, 0.0)
        end
    else
        for i in 1:(N_s + 1)
            v[i] = max(K - s_grid[i], 0.0)
        end
    end
    
    # Backward in time (from T to 0)
    # Using a simplified explicit finite difference for demonstration of PDE logic
    # In a production "enhanced" model, one would use Crank-Nicolson or PSOR for early exercise
    
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
        # Boundary conditions at S=0 and S=S_max
        if type == "call"
            v_next[1] = 0.0
            v_next[N_s+1] = s_grid[N_s+1] - K * exp(-r * (j * dt))
        else
            v_next[1] = K * exp(-r * (j * dt))
            v_next[N_s+1] = 0.0
        end
        v = v_next
    end
    
    # Interpolate to find the price at the current spot S
    itp = LinearInterpolation(s_grid, v)
    return itp(S)
end

# Enhanced BSM logic for different durations
function enhanced_bsm_american(ticker::String, type::String, duration::String, S::Float64, K::Float64, r::Float64, sigma::Float64)
    # Duration mapping to years
    T = 0.0
    if duration == "weekly"
        T = 7.0 / 365.0
    elseif duration == "monthly"
        T = 30.0 / 365.0
    else
        T = 0.25 # Default 3 months
    end
    
    price = price_american_option_pde(type, S, K, r, sigma, T)
    return price
end
