const float diffuseRef  = 0.6;
const float specRef     = 0.4;
const float shininess   = 7.0;

varying vec4 P;
varying vec3 N;

void main (void)
{
        vec3 p = vec3 (gl_ModelViewMatrix * P);
        vec3 n = normalize (gl_NormalMatrix * N);
        vec3 v = normalize (-p);

        gl_FragColor = 0.25 * (n.y*vec4 (1.0, 0.98, 0.66, 1) + (1.0-n.y)*(0.01, 0.2, 0.55));

        for (int i = 0; i < 3; i++)
        {
                vec3 l = normalize (gl_LightSource[i].position.xyz - p.xyz);
                float diffuse   = max (dot (l, n), 0.0);
                vec3 r = reflect (-l, n);

                float spec = max(dot(r, v), 0.0);
                spec = pow (spec, shininess);
                spec = max (0.0, spec);

                vec4 LightContribution =  diffuseRef * diffuse * gl_LightSource[i].diffuse +
                                          specRef * spec * gl_LightSource[i].specular;

                gl_FragColor += vec4 (LightContribution.xyz, 1);
        }
}
 
