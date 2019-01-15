#pragma once
#include "SFML/Graphics.hpp"

class ParticleSystem : public sf::Drawable, public sf::Transformable
{

public:
	sf::Texture* texture;
	struct Particle
	{
	public:
		sf::Vector2f velocity;
		sf::Time lifetime;
		sf::Color color;
	};

	ParticleSystem(unsigned int count) :m_particles(count), m_vertices(sf::Points, count), m_lifetime(sf::seconds(3.f)), m_emitter(0.f, 0.f)
	{

	}

	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	//elapsed = delta time(dt in most cases)
	void update(sf::Time elapsed)
	{
		sf::Vector2f size(texture->getSize());
		sf::Vector2f half = size / 2.f;
		m_vertices.clear();

		sf::Color c;
		sf::Vector2f pos;
		for (std::size_t i = 0; i < m_particles.size(); ++i)
		{
			pos = m_particles[i].velocity + m_emitter;
			c = sf::Color::Red/*m_particles[i].color*/;

			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
			{
				resetParticle(i);
			}



			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			c.a = static_cast<sf::Uint8>(255 * std::max(ratio, 0.f));

			m_vertices.append(sf::Vertex(sf::Vector2f(pos.x - half.x, pos.y - half.y), c, sf::Vector2f(0.f, 0.f)));
			m_vertices.append(sf::Vertex(sf::Vector2f(pos.x + half.x, pos.y - half.y), c, sf::Vector2f(size.x, 0.f)));
			m_vertices.append(sf::Vertex(sf::Vector2f(pos.x + half.x, pos.y + half.y), c, sf::Vector2f(size.x, size.y)));
			m_vertices.append(sf::Vertex(sf::Vector2f(pos.x - half.x, pos.y + half.y), c, sf::Vector2f(0.f, size.y)));

			//// update the position of the corresponding vertex
			//m_vertices[i].position += p.velocity * elapsed.asSeconds();



		}
	}

protected:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{

		// apply the transform
		states.transform *= getTransform();


		states.texture = NULL/*texture*/;

		// draw the vertex array
		target.draw(m_vertices, states);
	}
	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360) * 3.14f / 180.f;
		float speed = (std::rand() % 50) + 50.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

		//// reset the position of the corresponding vertex
		//m_vertices[index].position = m_emitter;

	}


	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};