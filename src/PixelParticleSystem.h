#pragma once
#include "SFML/Graphics.hpp"

//Particle system for one pixel sized particles
class PixelParticleSystem : public sf::Drawable, public sf::Transformable
{

public:
	sf::Texture* texture;
	sf::Color color;
	struct Particle
	{
	public:
		sf::Vector2f velocity;
		sf::Time lifetime;

	};

	PixelParticleSystem(unsigned int count, sf::Color color) :m_particles(count), color(color), m_vertices(sf::Points, count), m_lifetime(sf::seconds(0.1f)), m_emitter(0.f, 0.f)
	{

	}

	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	void Stop()
	{
		m_particles.clear();
		m_particles.resize(0);
	}
	//elapsed = delta time(dt in most cases)
	void update(sf::Time elapsed)
	{
		for (std::size_t i = 0; i < m_particles.size(); ++i)
		{
			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
				resetParticle(i);

			// update the position of the corresponding vertex
			m_vertices[i].position += p.velocity * elapsed.asSeconds();

			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			m_vertices[i].color = color;
			m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
		}
	}

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{

		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture = NULL/*texture*/;

		// draw the vertex array
		target.draw(m_vertices, states);
	}
	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 50) + 5.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

		// reset the position of the corresponding vertex
		m_vertices[index].position = m_emitter;
	}


	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};
